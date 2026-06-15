#include "init_kernel.h"

/*
** Teleport a ray that struck portal `src` so it continues out of the partner
** portal `dst`. The mapping is the standard Portal transform: a 180-degree
** rotation about the destination's up axis, so a ray entering the front of one
** portal leaves the front of the other. The in-plane offset of the hit (its
** coordinates along the source axes) is reproduced on the destination, and the
** new origin is nudged off dst's face so the exit ray cannot immediately
** re-hit the portal surface. u, v are the ellipse semi-axes; normalize gives
** the orthonormal frame the offsets are measured in.
*/
t_ray portal_warp(t_object src, t_object dst, float3 p, float3 dir)
{
  float3 su = normalize(src.u);
  float3 sv = normalize(src.v);
  float3 du = normalize(dst.u);
  float3 dv = normalize(dst.v);
  float3 rel = p - src.center;
  float  x = dot(rel, su);
  float  y = dot(rel, sv);
  float  a = dot(dir, su);
  float  b = dot(dir, sv);
  float  c = dot(dir, src.normal);
  t_ray  r;

  r.origin = dst.center + du * (-x) + dv * y + dst.normal * 0.05f;
  r.dir = normalize(du * (-a) + dv * b + dst.normal * (-c));
  return (r);
}
