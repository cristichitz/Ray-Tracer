#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

typedef struct s_sphere {
  t_hittable  base;
  t_vec3      center;
  float       radius;
} t_sphere;

bool      hit_sphere(void *base, t_ray ray, float t_min, float t_max, t_hit_record *rec);
t_sphere* make_sphere(t_vec3 center, float radius);
#endif

