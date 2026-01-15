#include "ray.h"

t_ray ray(t_point origin, t_vec3 direction)
{
  t_ray ray;

  ray.orig = origin;
  ray.dir = direction;
  return (ray);
}

t_point  ray_at(t_ray r, double t)
{
  // origin + t * dir
  return add(r.orig, scale(r.dir, t));
}