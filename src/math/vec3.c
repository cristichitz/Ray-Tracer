#include "vec3.h"

t_vec3  vec3(float x, float y, float z)
{
  t_vec3 v;

  v.x = x;
  v.y = y;
  v.z = z;
  return (v);
}

t_vec3  add(t_vec3 v1, t_vec3 v2)
{
  return (vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
}

t_vec3  sub(t_vec3 v1, t_vec3 v2)
{
  return (vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
}

t_vec3  mult(t_vec3 v1, t_vec3 v2)
{
  return (vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
}

t_vec3  scale(t_vec3 v1, float t)
{
  return (vec3(t * v1.x, t * v1.y, t * v1.z));
}

float dot(t_vec3 v1, t_vec3 v2)
{
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); 
}

t_vec3  cross(t_vec3 u, t_vec3 v)
{
  return vec3(u.y * v.z - u.z * v.y,
              u.z * v.x - u.x * v.z,
              u.x * v.y - u.y * v.x);
}

t_vec3  unit_vec(t_vec3 v)
{
  float len = vec_len(v);
  if (len == 0)
    return (vec3(0, 0, 0));
  return (vec3(v.x / len, v.y / len, v.z / len));
}

float vec_squared(t_vec3 v) {
  return (v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec_len(t_vec3 vector) {
  return sqrt(vec_squared(vector));
}