#include "init_kernel.h"

/*
** Dispatch one primitive by tag. Used both by BVH leaves and the linear
** plane pass.
*/
static bool hit_one(t_object obj, t_ray ray, t_interval span, t_hit_record *rec)
{
  if (obj.type == OBJ_SPHERE)
    return (hit_sphere(obj, ray, span, rec));
  if (obj.type == OBJ_PLANE)
    return (hit_plane(obj, ray, span, rec));
  if (obj.type == OBJ_CYLINDER)
    return (hit_cylinder(obj, ray, span, rec));
  if (obj.type == OBJ_QUAD)
    return (hit_quad(obj, ray, span, rec));
  return (false);
}

/* 1/d with a floor, so -cl-fast-relaxed-math never sees a divide by zero. */
static float safe_inv(float d)
{
  if (fabs(d) < 1e-12f)
    return (copysign(1e12f, d));
  return (1.0f / d);
}

/* Ray/AABB slab test, bounded by the closest hit found so far. */
static bool hit_aabb(float3 bmin, float3 bmax, t_ray ray, float tmin, float tmax)
{
  float3 inv = (float3)(safe_inv(ray.dir.x), safe_inv(ray.dir.y),
                        safe_inv(ray.dir.z));
  float3 t0 = (bmin - ray.origin) * inv;
  float3 t1 = (bmax - ray.origin) * inv;
  float3 lo = fmin(t0, t1);
  float3 hi = fmax(t0, t1);
  float  tn = fmax(fmax(lo.x, lo.y), fmax(lo.z, tmin));
  float  tf = fmin(fmin(hi.x, hi.y), fmin(hi.z, tmax));

  return (tn <= tf);
}

/* Test prim[first .. first+count) and keep the closest hit. */
static bool hit_range(t_scene sc, int first, int count, t_ray ray,
                      float tmin, float *closest, t_hit_record *rec)
{
  t_hit_record temp;
  bool hit_anything = false;
  int  i = 0;

  while (i < count)
  {
    t_object obj = sc.objs[sc.prim[first + i]];
    if (hit_one(obj, ray, interval_init(tmin, *closest), &temp))
    {
      temp.obj_type = obj.type;
      hit_anything = true;
      *closest = temp.t;
      *rec = temp;
    }
    i++;
  }
  return (hit_anything);
}

/*
** Closest hit over the whole scene: a linear pass over the (few) planes,
** then an iterative depth-first BVH walk over everything else. OpenCL has
** no recursion, so the walk keeps its own stack of node indices; the build
** splits at the median on degenerate cases, so depth stays ~log2(n) and 32
** entries is plenty.
*/
bool  hit_objects(t_scene sc, t_ray ray, t_interval t, t_hit_record *rec)
{
  int   stack[32];
  int   sp = 0;
  float closest = t.max;
  bool  hit_anything;

  hit_anything = hit_range(sc, sc.plane_first, sc.plane_count, ray,
                           t.min, &closest, rec);
  if (sc.nnodes > 0)
    stack[sp++] = 0;
  while (sp > 0)
  {
    t_bvh_node node = sc.nodes[stack[--sp]];
    if (!hit_aabb(node.bmin, node.bmax, ray, t.min, closest))
      continue;
    if (node.count > 0)
    {
      if (hit_range(sc, node.left, node.count, ray, t.min, &closest, rec))
        hit_anything = true;
    }
    else if (sp < 30)
    {
      stack[sp++] = node.left;
      stack[sp++] = node.right;
    }
  }
  return (hit_anything);
}
