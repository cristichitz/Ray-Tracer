#include "rt_bonus.h"

/*
** Infinite plane. We precompute D = dot(normal, Q) on the host so the kernel
** (kernels/hit_plane_bonus.c) only does a ray-plane test. `normal` is assumed
** already unit length (the parser normalises it).
*/
t_object  make_obj_plane(cl_float3 point, cl_float3 normal, t_material mat)
{
  t_object o;

  memset(&o, 0, sizeof(o));
  o.type = OBJ_PLANE;
  o.center = point;
  o.normal = normal;
  o.d = dot(normal, point);
  o.material = mat;
  return (o);
}

/*
** Bounded quad spanned by edge vectors u, v from corner Q. Precompute the
** plane normal, D and the w vector (n / dot(n,n)) used for the planar
** alpha/beta inside-test in kernels/hit_quad_bonus.c.
*/
t_object  make_obj_quad(cl_float3 q, cl_float3 u, cl_float3 v, t_material mat)
{
  t_object  o;
  cl_float3 n;

  memset(&o, 0, sizeof(o));
  n = cross(u, v);
  o.type = OBJ_QUAD;
  o.center = q;
  o.u = u;
  o.v = v;
  o.normal = norm(n);
  o.d = dot(o.normal, q);
  o.w = divide(n, dot(n, n));
  o.material = mat;
  return (o);
}
