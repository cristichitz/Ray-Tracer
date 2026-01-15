#ifndef VEC3_H
# define VEC3_H

#include <math.h>

typedef struct s_vec3 {
  float x;
  float y; 
  float z;
} t_vec3;


typedef t_vec3  t_color;
typedef t_vec3  t_point;
// COLOR
int     color_to_int(t_color pixel_color);

t_vec3  vec3(float x, float y, float z);
t_vec3  add(t_vec3 v1, t_vec3 v2);
t_vec3  sub(t_vec3 v1, t_vec3 v2);
t_vec3  mult(t_vec3 v1, t_vec3 v2);
t_vec3  scale(t_vec3 v1, float t);
float   dot(t_vec3 v1, t_vec3 v2);
t_vec3  cross(t_vec3 u, t_vec3 v);
t_vec3  unit_vec(t_vec3 v);
float   vec_squared(t_vec3 v);
float   vec_len(t_vec3 vector);

#endif