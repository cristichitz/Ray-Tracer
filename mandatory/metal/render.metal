/*
** render.metal — Apple Metal compute kernel for miniRT path tracing.
**
** One GPU thread per pixel. The kernel runs the full iterative path tracer
** (no recursion — GPU-friendly loop over bounces), outputting RGBA bytes
** directly into the pixel buffer that MLX42 reads.
**
** Data structures here MUST match metal_types.h exactly.
** MSL struct layout: float members → 4-byte aligned, same as C structs
** with only float/uint members → zero CPU↔GPU copy overhead on M2 UMA.
*/

#include <metal_stdlib>
using namespace metal;

/* ======================================================================== */
/*  Data structures (mirror metal_types.h — scalar members only)            */
/* ======================================================================== */

struct MFloat3 { float x, y, z; };

struct MetalCam {
	MFloat3  origin;
	MFloat3  pixel00;
	MFloat3  px_w;
	MFloat3  px_h;
	MFloat3  ambient;
	MFloat3  background;
	MFloat3  light_pos;
	MFloat3  light_color;
	float    light_brightness;
	uint     has_light;
	uint     width;
	uint     height;
	uint     max_depth;
	uint     samples_per_pixel;
	uint     frame_count;
};

constant uint MOBJ_SPHERE   = 1u;
constant uint MOBJ_CYLINDER = 2u;
constant uint MOBJ_PLANE    = 3u;
constant uint MOBJ_QUAD     = 4u;

struct MetalObj {
	uint    type;
	MFloat3 center;
	MFloat3 normal;
	MFloat3 u, v, w;
	MFloat3 albedo;
	MFloat3 emission;
	float   radius;
	float   height;
	float   D;
	uint    is_emissive;
};

/* ======================================================================== */
/*  Helpers                                                                  */
/* ======================================================================== */

/* Lift an MFloat3 (struct) into a native MSL float3 for math. */
inline float3 v3(MFloat3 m) { return float3(m.x, m.y, m.z); }

/*
** Orient outward_n to face the incoming ray (matches ft_set_face_normal).
** Returns a normal that always points toward the ray origin side.
*/
inline float3 face_normal(float3 ray_dir, float3 outward_n)
{
	if (dot(ray_dir, outward_n) < 0.0f)
		return outward_n;
	return -outward_n;
}

/* ======================================================================== */
/*  Random number generator — PCG hash (no stdlib rand() on GPU)            */
/* ======================================================================== */

/*
** One-shot PCG hash: maps any uint → pseudo-random uint.
** Seeded per-pixel and per-frame so each pixel gets unique noise
** and the noise pattern changes every frame (temporal variation).
*/
uint pcg_hash(uint v)
{
	uint state = v * 747796405u + 2891336453u;
	uint word  = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

/* Advance seed and return a float in [0, 1). */
float rand_f(thread uint *seed)
{
	*seed = pcg_hash(*seed);
	return float(*seed) / float(0xFFFFFFFFu);
}

/*
** Uniform point on the unit sphere using spherical coordinates.
** O(1) — no rejection loop, no GPU thread divergence.
** Matches the intent of CPU random_unit_vector().
*/
float3 rand_unit_sphere(thread uint *seed)
{
	float theta   = 2.0f * M_PI_F * rand_f(seed);
	float cos_phi = 2.0f * rand_f(seed) - 1.0f;
	float sin_phi = sqrt(max(0.0f, 1.0f - cos_phi * cos_phi));
	return float3(sin_phi * cos(theta), sin_phi * sin(theta), cos_phi);
}

/* ======================================================================== */
/*  Hit record                                                               */
/* ======================================================================== */

struct HitResult {
	bool   did_hit;
	float  t;
	float3 p;
	float3 normal;      /* face-oriented (always points toward ray origin) */
	float3 albedo;
	float3 emission;
	bool   is_emissive;
};

/* ======================================================================== */
/*  Ray–object intersections                                                 */
/* ======================================================================== */

/*
** Ray–sphere intersection.
** Matches CPU hit_sphere() using the half-vector form:
**   oc  = center - ray.origin
**   h   = dot(dir, oc)          (note: oc = center - origin, not origin - center)
**   disc = h*h - a*c
*/
bool hit_sphere_obj(
	const device MetalObj *o,
	float3 ro, float3 rd,
	float t_min, float t_max,
	thread HitResult *rec)
{
	float3 center = v3(o->center);
	float3 oc  = center - ro;
	float  a   = dot(rd, rd);
	float  h   = dot(rd, oc);
	float  c   = dot(oc, oc) - o->radius * o->radius;
	float  disc = h * h - a * c;
	if (disc < 0.0f) return false;

	float sqrtd = sqrt(disc);
	float root  = (h - sqrtd) / a;
	if (root <= t_min || root >= t_max) {
		root = (h + sqrtd) / a;
		if (root <= t_min || root >= t_max) return false;
	}
	float3 p     = ro + root * rd;
	float3 out_n = (p - center) / o->radius;
	rec->did_hit    = true;
	rec->t          = root;
	rec->p          = p;
	rec->normal     = face_normal(rd, out_n);
	rec->albedo     = v3(o->albedo);
	rec->emission   = v3(o->emission);
	rec->is_emissive = (o->is_emissive != 0u);
	return true;
}

/*
** Ray–plane intersection. Matches CPU hit_plane().
**   denom = dot(normal, ray.dir)
**   t     = (D - dot(normal, ray.origin)) / denom
*/
bool hit_plane_obj(
	const device MetalObj *o,
	float3 ro, float3 rd,
	float t_min, float t_max,
	thread HitResult *rec)
{
	float3 n     = v3(o->normal);
	float  denom = dot(n, rd);
	if (fabs(denom) < 1e-8f) return false;
	float t = (o->D - dot(n, ro)) / denom;
	if (t <= t_min || t >= t_max) return false;

	rec->did_hit    = true;
	rec->t          = t;
	rec->p          = ro + t * rd;
	rec->normal     = face_normal(rd, n);
	rec->albedo     = v3(o->albedo);
	rec->emission   = v3(o->emission);
	rec->is_emissive = (o->is_emissive != 0u);
	return true;
}

/*
** Single-cap intersection helper. Matches CPU hit_cap().
** Both top and bottom caps use the cylinder axis as the outward normal —
** face_normal() flips it based on the incoming ray direction.
*/
bool hit_one_cap(
	float3 cap_center, float3 axis, float radius,
	float3 ro, float3 rd,
	float t_min, float t_max,
	thread float *out_t, thread float3 *out_p)
{
	float cap_d = dot(axis, cap_center);
	float denom = dot(axis, rd);
	if (fabs(denom) < 1e-8f) return false;
	float t = (cap_d - dot(axis, ro)) / denom;
	if (t <= t_min || t >= t_max) return false;
	float3 to_hit = ro + t * rd - cap_center;
	if (dot(to_hit, to_hit) > radius * radius) return false;
	*out_t = t;
	*out_p = ro + t * rd;
	return true;
}

/*
** Ray–cylinder intersection (body + two endcaps). Matches CPU hit_cylinder().
**
** Projected quadratic (body only, parallel-projected along axis):
**   w     = ray.origin - cylinder.center
**   dv    = dot(ray.dir, axis)
**   wv    = dot(w, axis)
**   a     = dot(dir,dir) - dv²
**   half_b = dot(dir,w) - dv*wv
**   c     = dot(w,w) - wv² - r²
**
** After a body hit, we shrink t_max before checking caps — so a cap is
** only reported if it's closer than the body. Same logic as CPU.
*/
bool hit_cylinder_obj(
	const device MetalObj *o,
	float3 ro, float3 rd,
	float t_min, float t_max,
	thread HitResult *rec)
{
	float3 center = v3(o->center);
	float3 axis   = v3(o->normal);
	float  half_h = o->height * 0.5f;

	float3 w  = ro - center;
	float  dv = dot(rd, axis);
	float  wv = dot(w, axis);
	float  a  = dot(rd, rd) - dv * dv;
	float  hb = dot(rd, w) - dv * wv;
	float  c  = dot(w, w) - wv * wv - o->radius * o->radius;
	float  disc = hb * hb - a * c;

	bool   hit_body = false;
	float  body_t   = t_max;
	float3 body_p, body_n;

	if (disc >= 0.0f && a > 1e-8f) {
		float sqrtd = sqrt(disc);
		float root  = (-hb - sqrtd) / a;
		float m     = wv + root * dv;
		if (root <= t_min || root >= t_max || m < -half_h || m > half_h) {
			root = (-hb + sqrtd) / a;
			m    = wv + root * dv;
		}
		if (root > t_min && root < t_max && m >= -half_h && m <= half_h) {
			body_p    = ro + root * rd;
			body_n    = normalize(body_p - center - m * axis);
			body_t    = root;
			hit_body  = true;
			t_max     = root;   /* shrink interval: caps must beat body */
		}
	}

	/* Check both end caps (top = +half_h, bottom = -half_h). */
	float  t1 = 0.0f, t2 = 0.0f;
	float3 p1, p2;
	bool cap1 = hit_one_cap(center + half_h * axis, axis, o->radius,
	                        ro, rd, t_min, t_max, &t1, &p1);
	bool cap2 = hit_one_cap(center - half_h * axis, axis, o->radius,
	                        ro, rd, t_min, t_max, &t2, &p2);

	/* Both caps use the same axis normal — matches CPU hit_cap() behaviour. */
	if (!hit_body && !cap1 && !cap2) return false;

	float  best_t;
	float3 best_p, best_n;
	if (cap1 && (!cap2 || t1 <= t2)) {
		best_t = t1; best_p = p1; best_n = face_normal(rd, axis);
	} else if (cap2) {
		best_t = t2; best_p = p2; best_n = face_normal(rd, axis);
	} else {
		best_t = body_t; best_p = body_p; best_n = face_normal(rd, body_n);
	}

	rec->did_hit    = true;
	rec->t          = best_t;
	rec->p          = best_p;
	rec->normal     = best_n;
	rec->albedo     = v3(o->albedo);
	rec->emission   = v3(o->emission);
	rec->is_emissive = (o->is_emissive != 0u);
	return true;
}

/*
** Ray–quad intersection. Matches CPU hit_quad().
** After the plane test, uses barycentric coords (alpha, beta in [0,1])
** to confirm the hit is inside the parallelogram.
*/
bool hit_quad_obj(
	const device MetalObj *o,
	float3 ro, float3 rd,
	float t_min, float t_max,
	thread HitResult *rec)
{
	float3 n     = v3(o->normal);
	float  denom = dot(n, rd);
	if (fabs(denom) < 1e-8f) return false;
	float t = (o->D - dot(n, ro)) / denom;
	if (t <= t_min || t >= t_max) return false;

	float3 hit_pt = ro + t * rd;
	float3 p     = hit_pt - v3(o->center);   /* center == Q for quads */
	float3 qu    = v3(o->u);
	float3 qv    = v3(o->v);
	float3 qw    = v3(o->w);
	float  alpha = dot(qw, cross(p, qv));
	float  beta  = dot(qw, cross(qu, p));
	if (alpha < 0.0f || alpha > 1.0f || beta < 0.0f || beta > 1.0f)
		return false;

	rec->did_hit    = true;
	rec->t          = t;
	rec->p          = hit_pt;
	rec->normal     = face_normal(rd, n);
	rec->albedo     = v3(o->albedo);
	rec->emission   = v3(o->emission);
	rec->is_emissive = (o->is_emissive != 0u);
	return true;
}

/* ======================================================================== */
/*  Scene traversal — find closest hit among all objects                     */
/* ======================================================================== */

HitResult hit_scene(
	const device MetalObj *objects,
	uint object_count,
	float3 ro, float3 rd,
	float t_min, float t_max)
{
	HitResult rec;
	rec.did_hit = false;
	float closest = t_max;

	for (uint i = 0; i < object_count; i++) {
		HitResult tmp;
		bool did = false;
		uint type = objects[i].type;
		if (type == MOBJ_SPHERE)
			did = hit_sphere_obj(&objects[i], ro, rd, t_min, closest, &tmp);
		else if (type == MOBJ_PLANE)
			did = hit_plane_obj(&objects[i], ro, rd, t_min, closest, &tmp);
		else if (type == MOBJ_CYLINDER)
			did = hit_cylinder_obj(&objects[i], ro, rd, t_min, closest, &tmp);
		else if (type == MOBJ_QUAD)
			did = hit_quad_obj(&objects[i], ro, rd, t_min, closest, &tmp);
		if (did) {
			closest = tmp.t;
			rec = tmp;
		}
	}
	return rec;
}

/* ======================================================================== */
/*  Main compute kernel — one thread per pixel                              */
/* ======================================================================== */

kernel void ray_trace(
	device uint8_t         *pixels       [[buffer(0)]],   /* RGBA output     */
	constant MetalCam      &cam          [[buffer(1)]],   /* per-frame data  */
	const device MetalObj  *objects      [[buffer(2)]],   /* scene objects   */
	constant uint          &object_count [[buffer(3)]],   /* # objects       */
	uint2 gid [[thread_position_in_grid]])
{
	/* Discard threads in padding cells (grid is rounded up to threadgroup). */
	if (gid.x >= cam.width || gid.y >= cam.height)
		return;

	/*
	** Seed: mix pixel coords + frame counter.
	** frame_count changes every game_loop tick → different noise each frame
	** (temporal variation makes path tracing converge visually).
	*/
	uint seed = gid.x * 1973u + gid.y * 9277u + cam.frame_count * 26699u;
	seed = pcg_hash(seed);

	float3 cam_origin = v3(cam.origin);
	float3 pixel00    = v3(cam.pixel00);
	float3 px_w       = v3(cam.px_w);
	float3 px_h       = v3(cam.px_h);
	float3 ambient    = v3(cam.ambient);
	float3 background = v3(cam.background);

	float3 accumulated = float3(0.0f);

	for (uint s = 0; s < cam.samples_per_pixel; s++) {
		/*
		** Jittered sub-pixel offset — matches CPU sample_square() which
		** returns a random offset in [-0.5, 0.5] for x and y.
		*/
		float  ox = rand_f(&seed) - 0.5f;
		float  oy = rand_f(&seed) - 0.5f;
		float3 pixel_sample = pixel00
		    + (float(gid.x) + ox) * px_w
		    + (float(gid.y) + oy) * px_h;

		float3 ro = cam_origin;
		float3 rd = pixel_sample - ro;   /* unnormalised — matches CPU get_ray */

		/*
		** Iterative path trace — replaces CPU's recursive get_ray_color().
		** GPU does not like recursion (stack depth is fixed and small).
		** 'throughput' accumulates the product of surface albedos along the path.
		*/
		float3 color      = float3(0.0f);
		float3 throughput = float3(1.0f);

		for (uint depth = 0; depth < cam.max_depth; depth++) {
			HitResult hit = hit_scene(objects, object_count, ro, rd,
			                          0.001f, INFINITY);

			if (!hit.did_hit) {
				color += throughput * background;
				break;
			}

			/*
			** Emission + ambient — mirrors CPU get_ray_color():
			**   color_from_emission = mat.emitted(...)
			**   color_from_ambient  = world.ambient * mat.albedo
			**   emission_total      = color_from_emission + color_from_ambient
			** (ambient added every bounce — matches CPU behaviour)
			*/
			color += throughput * (hit.emission + ambient * hit.albedo);

			if (hit.is_emissive) break;

			/*
			** Lambertian scatter — mirrors CPU scatter_lambertian():
			**   scatter_dir = rec.normal + random_unit_vector()
			**   if near_zero → use normal as fallback
			*/
			float3 scatter = hit.normal + rand_unit_sphere(&seed);
			if (length_squared(scatter) < 1e-8f)
				scatter = hit.normal;

			throughput *= hit.albedo;
			ro = hit.p + hit.normal * 1e-3f;   /* bias off surface */
			rd = scatter;
		}

		accumulated += color;
	}

	accumulated /= float(cam.samples_per_pixel);

	/*
	** Gamma correction: sqrt — matches write_color() on the CPU.
	** Clamp to [0, 0.999] before scaling to avoid overflow at exactly 1.0.
	*/
	accumulated = sqrt(clamp(accumulated, 0.0f, 0.999f));

	uint32_t r = uint32_t(accumulated.x * 256.0f);
	uint32_t g = uint32_t(accumulated.y * 256.0f);
	uint32_t b = uint32_t(accumulated.z * 256.0f);

	/*
	** Write RGBA bytes in the order expected by mlx_draw_pixel:
	**   pixel[0] = R, pixel[1] = G, pixel[2] = B, pixel[3] = A (0xFF)
	** This matches the colour format (R<<24)|(G<<16)|(B<<8)|A used by write_color().
	*/
	uint32_t idx = gid.y * cam.width + gid.x;
	pixels[idx * 4 + 0] = uint8_t(r);
	pixels[idx * 4 + 1] = uint8_t(g);
	pixels[idx * 4 + 2] = uint8_t(b);
	pixels[idx * 4 + 3] = 0xFFu;
}
