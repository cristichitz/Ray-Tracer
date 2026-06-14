#include "init_kernel.h"

#define NEE_PI 3.14159265358979f

/*
** Next-event estimation: sample a point on an emitter and cast one shadow ray
** toward it, instead of hoping a bounced ray randomly stumbles onto the light.
** A light is a light regardless of its shape: both quad and sphere emitters are
** area-sampled the same way here, so neither is noisier than the other. One
** emitter is chosen uniformly by reservoir sampling and the result is scaled by
** the emitter count. Returns the direct-light term WITHOUT the surface albedo
** (the caller multiplies that in); the 1/PI lambertian factor is folded in.
*/

/* Any emitter NEE can area-sample (a quad or a sphere with an emissive mat). */
int is_nee_light(__constant t_object *o)
{
  return (o->material.type == 2
      && (o->type == OBJ_QUAD || o->type == OBJ_SPHERE));
}

/*
** Uniform sample on the chosen emitter: writes the world point q and its
** outward normal nl, and returns the emitter's surface area. A sphere is
** sampled over its whole surface (back-facing samples are simply shadowed by
** the sphere itself), a quad over its parallelogram.
*/
float sample_light(t_object light, uint *seed, float3 *q, float3 *nl)
{
  float3 dir;

  if (light.type == OBJ_SPHERE)
  {
    dir = random_unit_vector(seed);
    *q = light.center + dir * light.radius;
    *nl = dir;
    return (4.0f * NEE_PI * light.radius * light.radius);
  }
  *q = light.center + light.u * random_float(0.0f, 1.0f, seed)
      + light.v * random_float(0.0f, 1.0f, seed);
  *nl = light.normal;
  return (length(cross(light.u, light.v)));
}

float3 direct_light(t_scene sc, t_hit_record rec, uint *seed)
{
  int           n = 0;
  int           idx = -1;
  t_object      light;
  float3        q;
  float3        nl;
  float3        to_l;
  float3        wl;
  float         dist2;
  float         dist;
  float         cos_surf;
  float         cos_light;
  float         area;
  t_ray         shadow;
  t_hit_record  tmp;
  int           i;

  i = 0;
  while (i < sc.count)
  {
    if (is_nee_light(&sc.objs[i]))
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
  area = sample_light(light, seed, &q, &nl);
  to_l = q - rec.p;
  dist2 = dot(to_l, to_l);
  dist = sqrt(dist2);
  wl = to_l / dist;
  cos_surf = dot(rec.normal, wl);
  cos_light = fabs(dot(nl, wl));
  if (cos_surf <= 0.0f || cos_light <= 1e-6f)
    return ((float3)(0.0f));
  shadow = make_ray(rec.p, wl);
  if (hit_objects(sc, shadow, interval_init(0.001f, dist - 0.001f), &tmp))
    return ((float3)(0.0f));
  // Clamp the inverse-square falloff: points nearly touching the light make
  // 1/dist2 explode into firefly pixels that linger in the accumulation.
  return (light.material.albedo
      * (cos_surf * cos_light * area * (float)n
        / (NEE_PI * fmax(dist2, 0.25f))));
}
