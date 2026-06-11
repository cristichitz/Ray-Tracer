#include "init_kernel.h"

float3 reflect(float3 v, float3 n)
{
  // v is the incident ray, 2*dot(v,n)*n is the component along the normal
  return (v - n * 2.0f * dot(v, n));
}

bool  scatter_lambertian(t_material *self, t_hit_record rec, float3 *attenuation,
                         t_ray *scattered, uint *seed)
{
  float3 scatter_direction = rec.normal + random_unit_vector(seed);

  if (near_zero(scatter_direction))
    scatter_direction = rec.normal;
  *scattered = make_ray(rec.p, scatter_direction);
  *attenuation = self->albedo;
  return (true);
}

bool  scatter_metal(t_material *self, t_ray r_in, t_hit_record rec,
                    float3 *attenuation, t_ray *scattered)
{
  float3 reflected = reflect(r_in.dir, rec.normal);

  *scattered = make_ray(rec.p, reflected);
  *attenuation = self->albedo;
  return (true);
}

float3 emitted(t_material *self)
{
  if (self->type == 2)
    return (self->albedo);
  return ((float3)(0.0f));
}
