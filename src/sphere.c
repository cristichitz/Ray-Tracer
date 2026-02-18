#include "rt_cpu.h"


bool  hit_sphere(t_sphere *self, t_ray ray, float t_min, float t_max, t_hit_record *rec)
{
  t_vec3 oc = sub(self->center, ray.origin);

  float a = dot(ray.dir, ray.dir);
  float h = dot(ray.dir, oc);
  float c = dot(oc, oc) - self->radius * self->radius;

  float discriminant = h*h - a * c;
  if (discriminant < 0)
    return (false);

  float sqrtd = sqrt(discriminant);

  float root = (h - sqrtd) / a;

  if (root <= t_min || t_max <= root)
  {
    root = (h + sqrtd) / a;
    if (root <= t_min || t_max <= root)
      return (false);
  }
  rec->t = root;

  //point at t in the ray direction
  rec->p = ray.at(&ray, rec->t);

  // Point - sphere_center is already a normal vector
  // Dividing by the radius makes it unit length aswell
  // PC / radius
  t_vec3 outward_normal = divide(sub(rec->p, self->center), self->radius);
  rec->set_face_normal(rec, ray, outward_normal);
  
  return (true);
}

void  init_sphere(t_data *data)
{
  t_sphere sphere;

  sphere.center = make_vec(0, 0, -1);
  sphere.radius = 0.5f;
  sphere.hit = hit_sphere;

  data->sphere = sphere;
}

t_sphere* make_sphere(t_vec3 center, float radius)
{
  t_sphere *s;

  s = malloc(sizeof(t_sphere));
  s->radius = radius;
  s->center = center;
  s->hit = hit_sphere;
  return (s);
}
