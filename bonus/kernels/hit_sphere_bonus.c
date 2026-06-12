#include "init_kernel.h"

bool  hit_sphere(t_object self, t_ray ray, t_interval t, t_hit_record *rec)
{
  float3 oc = self.center - ray.origin;

  float a = dot(ray.dir, ray.dir);
  float h = dot(ray.dir, oc);
  float c = dot(oc, oc) - self.radius * self.radius;

  float discriminant = h * h - a * c;
  if (discriminant < 0)
    return (false);

  float sqrtd = sqrt(discriminant);

  float root = (h - sqrtd) / a;
  if (!interval_surrounds(&t, root))
  {
    root = (h + sqrtd) / a;
    if (!interval_surrounds(&t, root))
      return (false);
  }

  rec->t = root;
  rec->p = ray_at(&ray, rec->t);
  rec->mat = self.material;

  // (point - center) / radius is already a unit-length outward normal
  float3 outward_normal = (rec->p - self.center) / self.radius;
  ft_set_face_normal(rec, ray, outward_normal);
  return (true);
}
