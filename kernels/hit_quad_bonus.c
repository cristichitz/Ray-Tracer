#include "init_kernel.h"

bool  is_interior(float a, float b, t_hit_record *rec)
{
  t_interval ui = interval_init(0.0f, 1.0f);

  if (!interval_contains(&ui, a) || !interval_contains(&ui, b))
    return (false);
  rec->u = a;
  rec->v = b;
  return (true);
}

bool  hit_quad(t_object self, t_ray ray, t_interval ray_t, t_hit_record *rec)
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

  if (!is_interior(alpha, beta, rec))
    return (false);
  rec->t = t;
  rec->p = intersection;
  rec->mat = self.material;
  ft_set_face_normal(rec, ray, self.normal);
  return (true);
}
