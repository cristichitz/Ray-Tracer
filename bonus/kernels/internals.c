#include "internals.h"

t_interval interval_init(float t_min, float t_max)
{
  t_interval i;

  i.min = t_min;
  i.max = t_max;
  i.size = t_max - t_min;
  return (i);
}

bool  interval_contains(t_interval *self, float x)
{
  return (self->min <= x && x <= self->max);
}

bool  interval_surrounds(t_interval *self, float x)
{
  return (self->min < x && x < self->max);
}

//Keeps the number strictly between the intervals min and max
float interval_clamp(t_interval *self, float x)
{
  if (x < self->min)
      return (self->min);
  if (x > self->max)
    return (self->max);
  return (x);
}

t_ray make_ray(float3 origin, float3 direction) {
  t_ray r;

  r.origin = origin;
  r.dir = direction;
  return (r);
}

float3  ray_at(t_ray *self, float t)
{
  return (self->origin + (self->dir * t));
}

void  ft_set_face_normal(t_hit_record *self, t_ray r, float3 outward_normal)
{
  // true if the ray comes from outside the sphere 
  // (as in the ray doesn t come from inside like in a lightbulb)
  self->front_face = dot(r.dir, outward_normal) < 0;
  if (self->front_face != true)
    outward_normal = outward_normal * -1.0f;
  self->normal = outward_normal;
}
