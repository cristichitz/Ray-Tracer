#include "vec3.h"

t_vec3 make_vec(float a, float b, float c)
{
  return (t_vec3){a, b, c};
}

t_vec3 add(t_vec3 a, t_vec3 b)
{
  return (t_vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

t_vec3 sub(t_vec3 a, t_vec3 b)
{
  return (t_vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

t_vec3 scale(t_vec3 a, float b)
{
  return (t_vec3){a.x * b, a.y * b, a.z * b};
}

t_vec3 divide(t_vec3 a, float b)
{
  return (t_vec3){a.x / b, a.y / b, a.z /b};
}

float dot(t_vec3 a, t_vec3 b)
{
  return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vec3 norm(t_vec3 a)
{
  float len = sqrtf(a.x *a.x + a.y * a.y + a.z * a.z);
  return make_vec(a.x / len, a.y / len, a.z / len);
}

void  print_vec(t_vec3 a)
{
  printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}
