#include "rt_bonus.h"


cl_float3 make_float3(float x, float y, float z)
{
    cl_float3 v;
    v.s[0] = x;
    v.s[1] = y;
    v.s[2] = z;
    v.s[3] = 0.0f;
    return (v);
}

float   length_squared(cl_float3 a)
{
  return (a.x * a.x + a.y * a.y + a.z * a.z);
}

cl_float3  random_vec_between(float min, float max)
{
  return make_float3(random_float(min, max), random_float(min, max), random_float(min, max));
}

cl_float3  random_unit_vector(void)
{
  while (1)
  {
    cl_float3 p = random_vec_between(-1.0f, 1.0f);
    float lensq = length_squared(p);
    if (1e-32 < lensq && lensq <= 1)
      return (divide(p, sqrtf(lensq)));
  }
}

cl_float3 random_on_hemisphere(cl_float3 normal)
{
  cl_float3 on_unit_sphere;

  on_unit_sphere = random_unit_vector();
  if (dot(on_unit_sphere, normal) > 0.0f)
    return (on_unit_sphere);
  else
    return scale(on_unit_sphere, -1.0f);
}

bool  near_zero(cl_float3 a)
{
  float s;
  
  s = 1e-8;
  return (fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s);
}

cl_float3 mult(cl_float3 a, cl_float3 b)
{
  return (make_float3(a.x * b.x, a.y * b.y, a.z * b.z));
}

cl_float3 add(cl_float3 a, cl_float3 b)
{
  return (make_float3(a.x + b.x, a.y + b.y, a.z + b.z));
}

cl_float3 sub(cl_float3 a, cl_float3 b)
{
  return (make_float3(a.x - b.x, a.y - b.y, a.z - b.z));
}

cl_float3 scale(cl_float3 a, float b)
{
  return (make_float3(a.x * b, a.y * b, a.z * b));
}

cl_float3 divide(cl_float3 a, float b)
{
  return (make_float3(a.x / b, a.y / b, a.z /b));
}

float dot(cl_float3 a, cl_float3 b)
{
  return (a.x * b.x + a.y * b.y + a.z * b.z);
}

cl_float3 cross(cl_float3 a, cl_float3 b)
{
  return (make_float3(a.y * b.z - a.z * b.y,
                      a.z * b.x - a.x * b.z,
                      a.x * b.y - a.y * b.x));
}

cl_float3 norm(cl_float3 a)
{
  float len = sqrtf(a.x *a.x + a.y * a.y + a.z * a.z);
  return make_float3(a.x / len, a.y / len, a.z / len);
}

void  print_vec(cl_float3 a)
{
  printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}
