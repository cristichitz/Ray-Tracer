#ifndef RAY_H
# define RAY_H

#include "vec3.h"

typedef struct s_ray
{
  t_point  orig;
  t_vec3  dir;

} t_ray;

t_ray   ray(t_point origin, t_vec3 direction);
t_point  ray_at(t_ray r, double t);
#endif