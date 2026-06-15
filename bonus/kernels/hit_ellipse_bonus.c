#include "init_kernel.h"

/*
** Oriented ellipse (a flat disk with two perpendicular semi-axes). Same plane
** intersection as a quad, but `center` is the true center and u, v are the
** semi-axis vectors, so the interior test is the unit-disk inequality
** (alpha/|u|)^2 + (beta/|v|)^2 <= 1. alpha,beta come out as the coordinates of
** the hit along u and v exactly as in hit_quad (w = n / dot(n,n)).
*/
bool  hit_ellipse(t_object self, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
  float denom = dot(self.normal, ray.dir);
  if (fabs(denom) < 1e-8f)
    return (false);

  float t = (self.d - dot(self.normal, ray.origin)) / denom;
  if (!interval_contains(&ray_t, t))
    return (false);

  float3 intersection = ray_at(&ray, t);
  float3 p = intersection - self.center;
  float alpha = dot(self.w, cross(p, self.v));
  float beta = dot(self.w, cross(self.u, p));

  if (alpha * alpha + beta * beta > 1.0f)
    return (false);
  rec->u = alpha;
  rec->v = beta;
  rec->t = t;
  rec->p = intersection;
  rec->mat = self.material;
  ft_set_face_normal(rec, ray, self.normal);
  return (true);
}
