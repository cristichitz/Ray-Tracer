#include "hittable.h"

bool  scatter_lambertian(t_material *self, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered)
{
  t_vec3 scatter_direction;

  (void)r_in;
  scatter_direction = add(rec.normal, random_unit_vector());
  if (near_zero(scatter_direction))
    scatter_direction = rec.normal;

  *scattered = make_ray(rec.p, scatter_direction);
  *attenuation = self->albedo;
  return (true);
}

t_vec3 reflect(t_vec3 v, t_vec3 n)
{
  t_vec3 b;
  // v is the incident ray and - 2* dot(b, n) * n is the equivalent reflected ray
  b = scale(n, 2 * dot(v, n));
  return sub(v, b);
}

bool scatter_metal(t_material *self, t_ray r_in, t_hit_record rec, t_vec3 *attenuation, t_ray *scattered)
{
  t_vec3 reflected;

  reflected = reflect(r_in.dir, rec.normal);
  *scattered = make_ray(rec.p, reflected);
  *attenuation = self->albedo;
  return (true);
}

t_material  init_lambertian(t_vec3 color)
{
  t_material new_mat;

  new_mat.albedo = color;
  new_mat.scatter = scatter_lambertian;
  return (new_mat);
}

t_material init_metal(t_vec3 col)
{
   t_material new_mat;

   new_mat.albedo = col;
   new_mat.scatter = scatter_metal;
   return (new_mat);
}
