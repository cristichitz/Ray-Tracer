#include "init_kernel.h"

#define NEE_PI 3.14159265358979f

/*
** Next-event estimation: instead of hoping a bounced ray randomly hits the
** light, sample a point on a quad light directly and cast one shadow ray.
** Returns the direct-light term WITHOUT the surface albedo and WITHOUT the
** 1/PI lambertian factor folded out (caller multiplies by albedo; the 1/PI is
** included here). If there are several quad lights, one is chosen uniformly by
** reservoir sampling and the result is scaled by the light count.
**
** Only quad emitters are sampled here; sphere lights (from the L directive)
** keep being found by the normal bounce path, so the caller still counts their
** emission on diffuse hits.
*/
float3 direct_light(t_scene sc, t_hit_record rec, uint *seed)
{
  int       n = 0;
  int       idx = -1;
  t_object  light;
  float3    q;
  float3    to_l;
  float     dist2;
  float     dist;
  float3    wl;
  float     cos_surf;
  float     cos_light;
  t_ray         shadow;
  t_hit_record  tmp;
  float         area;
  int           i;

  i = 0;
  while (i < sc.count)
  {
    if (sc.objs[i].type == OBJ_QUAD && sc.objs[i].material.type == 2)
    {
      n++;
      if (random_float(0.0f, 1.0f, seed) * (float)n < 1.0f)
        idx = i;
    }
    i++;
  }
  if (idx < 0)
    return ((float3)(0.0f));
  light = sc.objs[idx];
  q = light.center + light.u * random_float(0.0f, 1.0f, seed)
      + light.v * random_float(0.0f, 1.0f, seed);
  to_l = q - rec.p;
  dist2 = dot(to_l, to_l);
  dist = sqrt(dist2);
  wl = to_l / dist;
  cos_surf = dot(rec.normal, wl);
  cos_light = fabs(dot(light.normal, wl));
  if (cos_surf <= 0.0f || cos_light <= 1e-6f)
    return ((float3)(0.0f));
  shadow = make_ray(rec.p, wl);
  if (hit_objects(sc, shadow, interval_init(0.001f, dist - 0.001f), &tmp))
    return ((float3)(0.0f));
  area = length(cross(light.u, light.v));
  // Clamp the inverse-square falloff: points nearly touching the light make
  // 1/dist2 explode into firefly pixels that linger in the accumulation.
  return (light.material.albedo
      * (cos_surf * cos_light * area * (float)n
        / (NEE_PI * fmax(dist2, 0.25f))));
}
