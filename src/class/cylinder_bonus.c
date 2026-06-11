#include "rt_bonus.h"

/*
** Finite, capped cylinder. `center` is the midpoint of the axis and `axis` is
** the unit direction (the parser normalises it). The kernel
** (kernels/hit_cylinder_bonus.c) works from the midpoint + half-height, so
** d_top is kept only for reference.
*/
t_object  make_obj_cylinder(cl_float3 center, cl_float3 axis, float radius,
                            float height, t_material mat)
{
  t_object o;

  memset(&o, 0, sizeof(o));
  o.type = OBJ_CYLINDER;
  o.center = center;
  o.normal = axis;
  o.radius = radius;
  o.height = height;
  o.d_top = dot(axis, center) + height / 2.0f;
  o.material = mat;
  return (o);
}
