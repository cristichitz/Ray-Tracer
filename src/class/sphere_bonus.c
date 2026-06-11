#include "rt_bonus.h"

/*
** Host-side builders for the flat t_object array uploaded to the GPU.
** The actual ray/primitive intersection math lives in the kernels
** (kernels/hit_sphere_bonus.c and friends); here we only fill structs.
*/
t_object  make_obj_sphere(cl_float3 center, float radius, t_material mat)
{
  t_object s;

  s.type = OBJ_SPHERE;
  s.center = center;
  s.radius = radius;
  s.material = mat;
  return (s);
}
