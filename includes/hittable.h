#ifndef HITTABLE_H
#define HITTABLE_H

#include <stdlib.h>
#include <sys/time.h>
#include "libft.h"
#include "vec3.h"
#include "interval.h"

typedef struct s_ray {
  t_vec3 origin;
  t_vec3 dir;
  t_vec3 (*at)(struct s_ray *self, float t);
} t_ray;

typedef struct s_hit_record {
  t_vec3  p;
  t_vec3  normal;
  float   t;
  bool    front_face; 
  void    (*set_face_normal)(struct s_hit_record *self, t_ray ray, t_vec3 outward_normal);
} t_hit_record;

// Base class 
typedef struct s_hittable {
  bool  (*hit)(void *object, t_ray ray, t_interval t, t_hit_record *rec);
  void  (*destroy)(void *object);
} t_hittable;

t_vec3    make_vec(float a, float b, float c);
// Ray
t_ray     make_ray(t_vec3 origin, t_vec3 direction);
// Hittable
void      ft_set_face_normal(t_hit_record *self, t_ray r, t_vec3 outward_normal);
#endif
