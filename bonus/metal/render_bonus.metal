/* ************************************************************************** */
/*                                                                            */
/*   render_bonus.metal                                                       */
/*                                                                            */
/*   Metal Shading Language port of the bonus OpenCL path tracer.             */
/*   All 15 kernel source files merged into one compute shader.               */
/*                                                                            */
/* ************************************************************************** */

#include <metal_stdlib>
using namespace metal;

/* ------------------------------------------------------------------ */
/*  Shared types – mirrors internals.h / init_kernel.h on the host    */
/* ------------------------------------------------------------------ */

#define RAND_MAX_U 2147483647u

#define OBJ_SPHERE   0
#define OBJ_PLANE    1
#define OBJ_CYLINDER 2
#define OBJ_QUAD     3

struct t_interval { float mn; float mx; float size; };

struct t_ray { float3 origin; float3 dir; };

struct t_material {
	float3 albedo;
	int    type;
	int    dynamic;
	float  density;
	float  restitution;
	float  friction;
};

struct t_hit_record {
	float3     p;
	float3     normal;
	float      t;
	float      u;
	float      v;
	bool       front_face;
	int        obj_type;
	t_material mat;
};

struct t_object {
	int        type;
	float3     center;
	float3     normal;
	float3     u;
	float3     v;
	float3     w;
	float      radius;
	float      height;
	float      d;
	float      d_top;
	t_material material;
};

struct t_bvh_node {
	float3 bmin;
	float3 bmax;
	int    left;
	int    right;
	int    count;
	int    pad;
};

struct t_image {
	int    width;
	int    height;
	float  aspect_ratio;
	float  viewport_height;
	float  viewport_width;
	float  focal_length;
	float3 horizontal;
	float3 vertical;
	int    samples_per_pixel;
	float  pixel_samples_scale;
	int    max_depth;
	float3 origin;
	float3 pixel00_loc;
	float3 pixel_delta_u;
	float3 pixel_delta_v;
	float3 background;
};

struct t_sphere_var {
	float a; float h; float c;
	float discriminant; float sqrtd; float root;
};

struct t_cyl_var {
	float3 w;
	float a; float half_b; float c;
	float dv; float wv;
	float discriminant; float sqrtd; float root; float m;
};

struct t_cap {
	float3     center;
	float3     normal;
	float      radius;
	t_material mat;
};

struct t_scat {
	float3 attenuation;
	t_ray  scattered;
};

struct t_path {
	t_ray  ray;
	float3 throughput;
	float3 out;
	bool   count_emission;
};

struct t_trace {
	t_ray        ray;
	t_hit_record rec;
	float        tmin;
	float        closest;
};

struct t_scene {
	constant t_object       *objs;
	constant t_bvh_node     *nodes;
	constant int            *prim;
	int                     count;
	int                     nnodes;
	int                     plane_first;
	int                     plane_count;
};

/* Uniform block passed as a single buffer from the host */
struct t_uniforms {
	t_image frame;
	int     obj_count;
	int     frame_index;
	int     nnodes;
	int     plane_first;
	int     plane_count;
};

/* ------------------------------------------------------------------ */
/*  interval_bonus.c                                                  */
/* ------------------------------------------------------------------ */

static t_interval interval_init(float t_min, float t_max)
{
	t_interval i;
	i.mn   = t_min;
	i.mx   = t_max;
	i.size = t_max - t_min;
	return i;
}

static bool interval_contains(thread t_interval *self, float x)
{
	return (self->mn <= x && x <= self->mx);
}

static bool interval_surrounds(thread t_interval *self, float x)
{
	return (self->mn < x && x < self->mx);
}

static float interval_clamp(thread t_interval *self, float x)
{
	if (x < self->mn) return self->mn;
	if (x > self->mx) return self->mx;
	return x;
}

/* ------------------------------------------------------------------ */
/*  math_bonus.c                                                      */
/* ------------------------------------------------------------------ */

static float safe_inv(float d)
{
	if (abs(d) < 1e-12f)
		return copysign(1e12f, d);
	return 1.0f / d;
}

/* ------------------------------------------------------------------ */
/*  ray_bonus.c                                                       */
/* ------------------------------------------------------------------ */

static t_ray make_ray(float3 origin, float3 direction)
{
	t_ray r;
	r.origin = origin;
	r.dir    = direction;
	return r;
}

static float3 ray_at(thread t_ray *self, float t)
{
	return self->origin + self->dir * t;
}

static void ft_set_face_normal(thread t_hit_record *self,
	t_ray r, float3 outward_normal)
{
	self->front_face = dot(r.dir, outward_normal) < 0;
	if (!self->front_face)
		outward_normal = outward_normal * -1.0f;
	self->normal = outward_normal;
}

/* ------------------------------------------------------------------ */
/*  random_bonus.c                                                    */
/* ------------------------------------------------------------------ */

static uint pcg_hash(thread uint *seed)
{
	uint state = *seed;
	*seed = state * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

static float random_float(float mn, float mx, thread uint *seed)
{
	float random_unorm = (float)pcg_hash(seed) / (float)0xFFFFFFFFu;
	return mn + (mx - mn) * random_unorm;
}

static float3 random_vec_between(float mn, float mx, thread uint *seed)
{
	return float3(random_float(mn, mx, seed),
		random_float(mn, mx, seed),
		random_float(mn, mx, seed));
}

static float3 random_unit_vector(thread uint *seed)
{
	float3 p;
	float lensq;
	for (;;) {
		p = random_vec_between(-1.0f, 1.0f, seed);
		lensq = dot(p, p);
		if (1e-32f < lensq && lensq <= 1.0f)
			return p / sqrt(lensq);
	}
}

static bool near_zero(float3 a)
{
	float s = 1e-8;
	return (abs(a.x) < s) && (abs(a.y) < s) && (abs(a.z) < s);
}

/* ------------------------------------------------------------------ */
/*  hit_sphere_bonus.c                                                */
/* ------------------------------------------------------------------ */

static void get_sphere_uv(const float3 p, thread float *u, thread float *v)
{
	float theta = acos(-p.y);
	float phi   = atan2(-p.z, p.x) + M_PI_F;
	*u = phi / (2.0f * M_PI_F);
	*v = theta / M_PI_F;
}

static float get_sphere_root(thread t_sphere_var *v, thread t_interval *t)
{
	v->discriminant = v->h * v->h - v->a * v->c;
	if (v->discriminant < 0)
		return -1;
	v->sqrtd = sqrt(v->discriminant);
	v->root  = (v->h - v->sqrtd) / v->a;
	if (!interval_surrounds(t, v->root))
	{
		v->root = (v->h + v->sqrtd) / v->a;
		if (!interval_surrounds(t, v->root))
			return -1;
	}
	return v->root;
}

static bool hit_sphere(constant t_object *self, t_ray ray,
	t_interval t, thread t_hit_record *rec)
{
	float3       oc = self->center - ray.origin;
	t_sphere_var v;
	float3       outward_normal;

	v.a = dot(ray.dir, ray.dir);
	v.h = dot(ray.dir, oc);
	v.c = dot(oc, oc) - self->radius * self->radius;
	if (get_sphere_root(&v, &t) < 0)
		return false;
	rec->t   = v.root;
	rec->p   = ray_at(&ray, rec->t);
	rec->mat = self->material;
	outward_normal = (rec->p - self->center) / self->radius;
	ft_set_face_normal(rec, ray, outward_normal);
	get_sphere_uv(outward_normal, &rec->u, &rec->v);
	return true;
}

/* ------------------------------------------------------------------ */
/*  hit_plane_bonus.c                                                 */
/* ------------------------------------------------------------------ */

static bool hit_plane(constant t_object *self, t_ray ray,
	t_interval ray_t, thread t_hit_record *rec)
{
	float denom = dot(self->normal, ray.dir);
	if (abs(denom) < 1e-8f)
		return false;
	float t = (self->d - dot(self->normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return false;
	rec->p   = ray_at(&ray, t);
	rec->t   = t;
	rec->mat = self->material;
	ft_set_face_normal(rec, ray, self->normal);
	return true;
}

/* ------------------------------------------------------------------ */
/*  hit_quad_bonus.c                                                  */
/* ------------------------------------------------------------------ */

static bool is_interior(float a, float b, thread t_hit_record *rec)
{
	t_interval ui = interval_init(0.0f, 1.0f);
	if (!interval_contains(&ui, a) || !interval_contains(&ui, b))
		return false;
	rec->u = a;
	rec->v = b;
	return true;
}

static bool quad_intersection(constant t_object *self, float3 intersection,
	thread t_hit_record *rec)
{
	float3 p     = intersection - self->center;
	float  alpha = dot(self->w, cross(p, self->v));
	float  beta  = dot(self->w, cross(self->u, p));
	if (!is_interior(alpha, beta, rec))
		return false;
	rec->p   = intersection;
	rec->mat = self->material;
	return true;
}

static bool hit_quad(constant t_object *self, t_ray ray,
	t_interval ray_t, thread t_hit_record *rec)
{
	float denom = dot(self->normal, ray.dir);
	if (abs(denom) < 1e-8f)
		return false;
	float t = (self->d - dot(self->normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return false;
	float3 intersection = ray_at(&ray, t);
	if (!quad_intersection(self, intersection, rec))
		return false;
	rec->t = t;
	ft_set_face_normal(rec, ray, self->normal);
	return true;
}

/* ------------------------------------------------------------------ */
/*  hit_cyl_caps_bonus.c                                              */
/* ------------------------------------------------------------------ */

static t_cap make_cap(constant t_object *self, float offset,
	float normal_mult)
{
	t_cap cap;
	cap.center = self->center + (self->normal * offset);
	cap.normal = self->normal * normal_mult;
	cap.radius = self->radius;
	cap.mat    = self->material;
	return cap;
}

static bool hit_cap(t_cap cap, t_ray ray, t_interval ray_t,
	thread t_hit_record *rec)
{
	float cap_d = dot(cap.normal, cap.center);
	float denom = dot(cap.normal, ray.dir);
	if (abs(denom) < 1e-8)
		return false;
	float t = (cap_d - dot(cap.normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return false;
	float3 to_hit = ray_at(&ray, t) - cap.center;
	if (dot(to_hit, to_hit) > (cap.radius * cap.radius))
		return false;
	rec->p   = ray_at(&ray, t);
	rec->t   = t;
	rec->mat = cap.mat;
	ft_set_face_normal(rec, ray, cap.normal);
	return true;
}

static bool hit_caps(constant t_object *self, t_ray ray, t_interval ray_t,
	thread t_hit_record *rec)
{
	t_hit_record tmp;
	bool         hit_any = false;
	float        half_h  = self->height / 2.0f;

	if (hit_cap(make_cap(self, half_h, 1.0f), ray, ray_t, &tmp))
	{
		*rec      = tmp;
		ray_t.mx  = tmp.t;
		hit_any   = true;
	}
	if (hit_cap(make_cap(self, -half_h, -1.0f), ray, ray_t, &tmp))
	{
		*rec    = tmp;
		hit_any = true;
	}
	return hit_any;
}

/* ------------------------------------------------------------------ */
/*  hit_cylinder_bonus.c                                              */
/* ------------------------------------------------------------------ */

static void init_cyl_var(constant t_object *self, t_ray ray, thread t_cyl_var *v)
{
	v->w      = ray.origin - self->center;
	v->dv     = dot(ray.dir, self->normal);
	v->wv     = dot(v->w, self->normal);
	v->a      = dot(ray.dir, ray.dir) - v->dv * v->dv;
	v->half_b = dot(ray.dir, v->w) - v->dv * v->wv;
	v->c      = dot(v->w, v->w) - v->wv * v->wv
		- self->radius * self->radius;
}

static float get_cyl_root(constant t_object *self, thread t_cyl_var *v,
	thread t_interval *t)
{
	float half_h = self->height / 2;
	v->discriminant = v->half_b * v->half_b - v->a * v->c;
	if (v->discriminant < 0)
		return -1;
	v->sqrtd = sqrt(v->discriminant);
	v->root  = (-v->half_b - v->sqrtd) / v->a;
	v->m     = v->wv + v->root * v->dv;
	if (!interval_surrounds(t, v->root) || v->m < -half_h || v->m > half_h)
	{
		v->root = (-v->half_b + v->sqrtd) / v->a;
		v->m    = v->wv + v->root * v->dv;
	}
	if (interval_surrounds(t, v->root) && v->m >= -half_h && v->m <= half_h)
		return v->root;
	return -1;
}

static bool hit_cyl_body(constant t_object *self, t_ray ray,
	t_interval t, thread t_hit_record *rec)
{
	t_cyl_var v;
	init_cyl_var(self, ray, &v);
	if (get_cyl_root(self, &v, &t) < 0)
		return false;
	rec->t   = v.root;
	rec->p   = ray_at(&ray, v.root);
	float3 outward_normal = normalize((rec->p - self->center)
		- self->normal * v.m);
	ft_set_face_normal(rec, ray, outward_normal);
	rec->mat = self->material;
	return true;
}

static bool hit_cylinder(constant t_object *self, t_ray ray,
	t_interval t, thread t_hit_record *rec)
{
	bool hit_body = hit_cyl_body(self, ray, t, rec);
	if (hit_body)
		t.mx = rec->t;
	if (hit_caps(self, ray, t, rec))
		return true;
	return hit_body;
}

/* ------------------------------------------------------------------ */
/*  objects_bonus.c  (BVH traversal)                                  */
/* ------------------------------------------------------------------ */

static bool hit_one(constant t_object *obj, t_ray ray,
	t_interval span, thread t_hit_record *rec)
{
	if (obj->type == OBJ_SPHERE)
		return hit_sphere(obj, ray, span, rec);
	if (obj->type == OBJ_PLANE)
		return hit_plane(obj, ray, span, rec);
	if (obj->type == OBJ_CYLINDER)
		return hit_cylinder(obj, ray, span, rec);
	if (obj->type == OBJ_QUAD)
		return hit_quad(obj, ray, span, rec);
	return false;
}

static bool hit_aabb(float3 bmin, float3 bmax, t_ray ray, t_interval span)
{
	float3 inv = float3(safe_inv(ray.dir.x), safe_inv(ray.dir.y),
		safe_inv(ray.dir.z));
	float3 lo = min((bmin - ray.origin) * inv,
		(bmax - ray.origin) * inv);
	float3 hi = max((bmin - ray.origin) * inv,
		(bmax - ray.origin) * inv);
	return max(max(lo.x, lo.y), max(lo.z, span.mn))
		<= min(min(hi.x, hi.y), min(hi.z, span.mx));
}

static bool hit_range(t_scene sc, thread t_trace *tr, int first, int cnt)
{
	t_hit_record        tmp;
	bool                hit_anything = false;
	constant t_object  *obj;

	for (int i = 0; i < cnt; i++)
	{
		obj = &sc.objs[sc.prim[first + i]];
		if (hit_one(obj, tr->ray,
			interval_init(tr->tmin, tr->closest), &tmp))
		{
			tmp.obj_type = obj->type;
			hit_anything = true;
			tr->closest  = tmp.t;
			tr->rec      = tmp;
		}
	}
	return hit_anything;
}

static bool bvh_walk(t_scene sc, thread t_trace *tr,
	thread int *stack, int sp)
{
	bool       any = false;
	t_bvh_node node;

	while (sp > 0)
	{
		node = sc.nodes[stack[--sp]];
		if (!hit_aabb(node.bmin, node.bmax, tr->ray,
			interval_init(tr->tmin, tr->closest)))
			continue;
		if (node.count > 0)
		{
			if (hit_range(sc, tr, node.left, node.count))
				any = true;
		}
		else if (sp < 30)
		{
			stack[sp++] = node.left;
			stack[sp++] = node.right;
		}
	}
	return any;
}

static bool hit_objects(t_scene sc, t_ray ray, t_interval t,
	thread t_hit_record *rec)
{
	int     stack[32];
	t_trace tr;
	bool    hit;

	tr.ray     = ray;
	tr.tmin    = t.mn;
	tr.closest = t.mx;
	hit = hit_range(sc, &tr, sc.plane_first, sc.plane_count);
	if (sc.nnodes > 0)
	{
		stack[0] = 0;
		if (bvh_walk(sc, &tr, stack, 1))
			hit = true;
	}
	if (hit)
		*rec = tr.rec;
	return hit;
}

/* ------------------------------------------------------------------ */
/*  scatter_bonus.c                                                   */
/* ------------------------------------------------------------------ */

static float3 reflect_vec(float3 v, float3 n)
{
	return v - n * 2.0f * dot(v, n);
}

static bool scatter_lambertian(thread t_material *self, t_hit_record rec,
	thread t_scat *scat, thread uint *seed)
{
	float3 scatter_direction = rec.normal + random_unit_vector(seed);
	if (near_zero(scatter_direction))
		scatter_direction = rec.normal;
	scat->scattered   = make_ray(rec.p, scatter_direction);
	scat->attenuation = self->albedo;
	return true;
}

static bool scatter_metal(thread t_material *self, t_ray r_in,
	t_hit_record rec, thread t_scat *scat)
{
	float3 reflected = reflect_vec(r_in.dir, rec.normal);
	scat->scattered   = make_ray(rec.p, reflected);
	scat->attenuation = self->albedo;
	return true;
}

static float3 emitted(thread t_material *self)
{
	if (self->type == 2)
		return self->albedo;
	return float3(0.0f);
}

/* ------------------------------------------------------------------ */
/*  nee_bonus.c  (Next-Event Estimation)                              */
/* ------------------------------------------------------------------ */

#define NEE_PI 3.14159265358979f

static float sample_light(constant t_object *light, thread uint *seed,
	thread float3 *q, thread float3 *nl)
{
	if (light->type == OBJ_SPHERE)
	{
		float3 dir = random_unit_vector(seed);
		*q  = light->center + dir * light->radius;
		*nl = dir;
		return 4.0f * NEE_PI * light->radius * light->radius;
	}
	*q  = light->center + light->u * random_float(0.0f, 1.0f, seed)
		+ light->v * random_float(0.0f, 1.0f, seed);
	*nl = light->normal;
	return length(cross(light->u, light->v));
}

static int pick_emitter(t_scene sc, thread uint *seed, thread int *n)
{
	int idx = -1;
	*n = 0;
	for (int i = 0; i < sc.count; i++)
	{
		if (sc.objs[i].material.type == 2
			&& (sc.objs[i].type == OBJ_QUAD
				|| sc.objs[i].type == OBJ_SPHERE))
		{
			(*n)++;
			if (random_float(0.0f, 1.0f, seed) * (float)(*n) < 1.0f)
				idx = i;
		}
	}
	return idx;
}

static float3 nee_shade(constant t_object *lt, float3 n,
	float3 nl, float3 wl)
{
	float dist2    = dot(wl, wl);
	wl             = wl / sqrt(dist2);
	float cos_surf  = dot(n, wl);
	float cos_light = abs(dot(nl, wl));
	if (cos_surf <= 0.0f || cos_light <= 1e-6f)
		return float3(0.0f);
	float area = length(cross(lt->u, lt->v));
	if (lt->type == OBJ_SPHERE)
		area = 4.0f * NEE_PI * lt->radius * lt->radius;
	return lt->material.albedo * (cos_surf * cos_light
		* area / (NEE_PI * max(dist2, 0.25f)));
}

static float3 nee_eval(t_scene sc, t_hit_record rec,
	constant t_object *light, thread uint *seed)
{
	float3       q, nl, wl;
	t_hit_record tmp;

	sample_light(light, seed, &q, &nl);
	wl = q - rec.p;
	if (dot(rec.normal, normalize(wl)) <= 0.0f)
		return float3(0.0f);
	if (hit_objects(sc, make_ray(rec.p, normalize(wl)),
		interval_init(0.001f, length(wl) - 0.001f), &tmp))
		return float3(0.0f);
	return nee_shade(light, rec.normal, nl, wl);
}

static float3 direct_light(t_scene sc, t_hit_record rec, thread uint *seed)
{
	int idx, n;
	idx = pick_emitter(sc, seed, &n);
	if (idx < 0)
		return float3(0.0f);
	constant t_object *light = &sc.objs[idx];
	return nee_eval(sc, rec, light, seed) * (float)n;
}

/* ------------------------------------------------------------------ */
/*  camera_bonus.c                                                    */
/* ------------------------------------------------------------------ */

static float3 sample_square(thread uint *seed)
{
	return float3(random_float(0.0f, 1.0f, seed) - 0.5f,
		random_float(0.0f, 1.0f, seed) - 0.5f, 0.0f);
}

static t_ray get_ray(constant t_image *frame, int x, int y, thread uint *seed)
{
	float3 offset       = sample_square(seed);
	float3 pixel_sample = (frame->pixel_delta_u * (x + offset.x))
		+ (frame->pixel_delta_v * (y + offset.y) + frame->pixel00_loc);
	float3 ray_origin   = frame->origin;
	float3 ray_dir      = pixel_sample - ray_origin;
	return make_ray(ray_origin, ray_dir);
}

/* ------------------------------------------------------------------ */
/*  color_bonus.c                                                     */
/* ------------------------------------------------------------------ */

static bool roulette(thread t_path *pt, thread uint *seed)
{
	float q = max(pt->throughput.x,
		max(pt->throughput.y, pt->throughput.z));
	if (random_float(0.0f, 1.0f, seed) > q)
		return true;
	pt->throughput /= q;
	return false;
}

static bool shade_hit(t_scene sc, thread t_hit_record *rec,
	thread t_path *pt, thread uint *seed)
{
	t_scat scat;

	if (pt->count_emission || rec->mat.type != 2
		|| (rec->obj_type != OBJ_QUAD && rec->obj_type != OBJ_SPHERE))
		pt->out += pt->throughput * emitted(&rec->mat);
	if (rec->mat.type == 1)
	{
		scatter_metal(&rec->mat, pt->ray, *rec, &scat);
		pt->count_emission = true;
	}
	else if (rec->mat.type == 0)
	{
		pt->out += pt->throughput * rec->mat.albedo
			* direct_light(sc, *rec, seed);
		scatter_lambertian(&rec->mat, *rec, &scat, seed);
		pt->count_emission = false;
	}
	else
		return false;
	pt->throughput *= scat.attenuation;
	pt->ray = scat.scattered;
	return true;
}

static float3 get_ray_color(t_scene sc, t_ray ray,
	constant t_image *f, thread uint *seed)
{
	t_path       pt;
	t_hit_record rec;

	pt.ray            = ray;
	pt.throughput     = float3(1.0f);
	pt.out            = float3(0.0f);
	pt.count_emission = true;
	for (int depth = 0; depth < f->max_depth; depth++)
	{
		if (!hit_objects(sc, pt.ray, interval_init(0.001f, INFINITY), &rec))
			return pt.out + pt.throughput * f->background;
		if (!shade_hit(sc, &rec, &pt, seed))
			break;
		if (depth > 3 && roulette(&pt, seed))
			break;
	}
	return pt.out;
}

static void write_color(device uchar4 *img_buffer, int id, float3 color)
{
	t_interval intensity = interval_init(0.0f, 0.999f);
	uchar4 c;
	c.x = (uchar)(256 * interval_clamp(&intensity, color.x));
	c.y = (uchar)(256 * interval_clamp(&intensity, color.y));
	c.z = (uchar)(256 * interval_clamp(&intensity, color.z));
	c.w = 255;
	img_buffer[id] = c;
}

/* ------------------------------------------------------------------ */
/*  render_bonus.c  (main kernel entry)                               */
/* ------------------------------------------------------------------ */

static float3 render_pixel(t_scene sc, constant t_image *f,
	int id, thread uint *seed)
{
	float3 color = float3(0.0f);
	for (int sample = 0; sample < f->samples_per_pixel; sample++)
	{
		t_ray r = get_ray(f, id % f->width, id / f->width, seed);
		color += min(get_ray_color(sc, r, f, seed), float3(10.0f));
	}
	return color * f->pixel_samples_scale;
}

static float3 accumulate(device float4 *accum, int id, float3 color,
	int frame_index)
{
	float3 acc;
	if (frame_index == 0)
		acc = color;
	else
		acc = accum[id].xyz + color;
	accum[id] = float4(acc, 1.0f);
	return acc;
}

kernel void render_kernel(
	device uchar4           *img_buffer  [[buffer(0)]],
	constant t_object       *objects     [[buffer(1)]],
	device float4           *accum       [[buffer(2)]],
	constant t_bvh_node     *nodes       [[buffer(3)]],
	constant int            *prim        [[buffer(4)]],
	constant t_uniforms     &uniforms    [[buffer(5)]],
	uint2                    gid         [[thread_position_in_grid]])
{
	constant t_image *frame = &uniforms.frame;
	int x = (int)gid.x;
	int y = (int)gid.y;
	if (x >= frame->width || y >= frame->height)
		return;
	int id = y * frame->width + x;

	t_scene sc;
	sc.objs        = objects;
	sc.nodes       = nodes;
	sc.prim        = prim;
	sc.count       = uniforms.obj_count;
	sc.nnodes      = uniforms.nnodes;
	sc.plane_first = uniforms.plane_first;
	sc.plane_count = uniforms.plane_count;

	uint seed = (uint)id + (uint)uniforms.frame_index * 9781u;
	pcg_hash(&seed);

	float3 color = render_pixel(sc, frame, id, &seed);
	color = accumulate(accum, id, color, uniforms.frame_index);
	write_color(img_buffer, id, color / (float)(uniforms.frame_index + 1));
}
