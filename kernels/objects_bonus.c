#include "init_kernel.h"

bool  hit_objects(__global t_object *objs, int count, t_ray ray,
                  t_interval t, t_hit_record *rec)
{
  t_hit_record temp_rec;
  bool  hit_anything = false;
  float closest_so_far = t.max;
  int   i = 0;

  while (i < count)
  {
    bool ok = false;
    t_object obj = objs[i];
    t_interval span = interval_init(t.min, closest_so_far);

    if (obj.type == OBJ_SPHERE)
      ok = hit_sphere(obj, ray, span, &temp_rec);
    else if (obj.type == OBJ_PLANE)
      ok = hit_plane(obj, ray, span, &temp_rec);
    else if (obj.type == OBJ_CYLINDER)
      ok = hit_cylinder(obj, ray, span, &temp_rec);
    else if (obj.type == OBJ_QUAD)
      ok = hit_quad(obj, ray, span, &temp_rec);

    if (ok)
    {
      temp_rec.obj_type = obj.type;
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
    i++;
  }
  return (hit_anything);
}
