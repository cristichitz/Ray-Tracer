#include "sphere.h"

bool  hit_sphere(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
  t_sphere *self;

  self = (t_sphere *)base;
  t_vec3 oc = sub(self->center, ray.origin);

  float a = dot(ray.dir, ray.dir);
  float h = dot(ray.dir, oc);
  float c = dot(oc, oc) - self->radius * self->radius;

  float discriminant = h*h - a * c;
  if (discriminant < 0)
    return (false);

  float sqrtd = sqrt(discriminant);

  float root = (h - sqrtd) / a;
  if (!t.surrounds(&t, root))
  {
    root = (h + sqrtd) / a;
    if (!t.surrounds(&t, root))
      return (false);
  }

  rec->t = root;
  //point at t in the ray direction
  rec->p = ray.at(&ray, rec->t);
  rec->mat = self->mat;

  // Point - sphere_center is already a normal vector
  // Dividing by the radius makes it unit length aswell
  t_vec3 outward_normal = divide(sub(rec->p, self->center), self->radius);
  rec->set_face_normal(rec, ray, outward_normal);
  
  return (true);
}

/* t_sphere* make_sphere(t_vec3 center, float radius, t_material mat) */
/* { */
/*   t_sphere *s; */
/**/
/*   s = malloc(sizeof(t_sphere)); */
/*   if (!s) */
/*     return (NULL); */
/*   s->radius = radius; */
/*   s->center = center; */
/*   s->mat = mat; */
/*   s->base.hit = hit_sphere; */
/*   return (s); */
/* } */
