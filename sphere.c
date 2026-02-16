#include "rt_cpu.h"

float hit_sphere(t_sphere *self, t_ray ray)
{
  // origin -> sphere_center 
  t_vec3 oc = sub(self->center, ray.origin);

  float a = dot(ray.dir, ray.dir);
  float b = -2.0f * dot(ray.dir, oc);
  float c = dot(oc, oc) - self->radius * self->radius;

  float discriminant = b*b - 4.0f * a * c;
  if (discriminant < 0)
    return (-1.0f);
  else
    return ((-b - sqrtf(discriminant)) / (2.0*a));
}

void  init_sphere(t_data *data)
{
  t_sphere sphere;

  sphere.center = make_vec(0, 0, -1);
  sphere.radius = 0.5f;
  sphere.hit = hit_sphere;

  data->sphere = sphere;
}
