#include "init_kernel.h"

bool  hit_sphere(t_sphere self, t_ray ray, t_interval t, t_hit_record *rec)
{

  float3 oc = self.center - ray.origin;

  float a = dot(ray.dir, ray.dir);
  float h = dot(ray.dir, oc);
  float c = dot(oc, oc) - self.radius * self.radius;

  float discriminant = h*h - a * c;
  if (discriminant < 0)
    return (false);

  float sqrtd = sqrt(discriminant);

  float root = (h - sqrtd) / a;
  if (!interval_surrounds(&t, root))
  {
    root = (h + sqrtd) / a;
    if (!interval_surrounds(&t, root))
      return (false);
  }

  rec->t = root;
  //point at t in the ray direction
  rec->p = ray_at(&ray, rec->t);
  rec->mat = self.material;

  // Point - sphere_center is already a normal vector
  // Dividing by the radius makes it unit length aswell
  float3 outward_normal = (rec->p - self.center) / self.radius;
  ft_set_face_normal(rec, ray, outward_normal);
  
  return (true);
}

bool  hit_objects(__global t_sphere *spheres, t_ray ray, t_interval t, t_hit_record *rec)
{ 
  t_hit_record temp_rec;

  bool hit_anything = false;
  float closest_so_far = t.max;

  int i;
  i = 0;
  while (i < 4)
  {
    if (hit_sphere(spheres[i], ray, interval_init(t.min, closest_so_far), &temp_rec))
    {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
    i++;
  }
  return hit_anything; 
}

bool  scatter_lambertian(t_material *self, t_ray r_in, t_hit_record rec, float3 *attenuation, t_ray *scattered, uint *seed)
{
  float3 scatter_direction;

  (void)r_in;
  scatter_direction = rec.normal + random_unit_vector(seed);
  if (near_zero(scatter_direction))
    scatter_direction = rec.normal;

*scattered = make_ray(rec.p, scatter_direction);
  *attenuation = self->albedo;
  return (true);
}

float3 reflect(float3 v, float3 n)
{
  float3 b;
  // v is the incident ray and - 2* dot(b, n) * n is the equivalent reflected ray
  b = n * 2.0f * dot(v, n);
  return v - b;
}

bool scatter_metal(t_material *self, t_ray r_in, t_hit_record rec, float3 *attenuation, t_ray *scattered)
{
  float3 reflected;

  reflected = reflect(r_in.dir, rec.normal);
  *scattered = make_ray(rec.p, reflected);
  *attenuation = self->albedo;
  return (true);
}

float3 get_ray_color(__global t_sphere *spheres, int max_depth, t_ray ray, uint *seed)
{
  t_hit_record  rec;
  float3  current_attenuation = (float3)(1.0f, 1.0f, 1.0f);

  for (int depth = 0; depth < max_depth; depth++)
  {
    if (hit_objects(spheres, ray, interval_init(0.001f, INFINITY), &rec))
    {
      t_ray   scattered;
      float3  attenuation;
      bool    did_scatter;

      did_scatter = false;

      if (rec.mat.type == 0)
        did_scatter = scatter_lambertian(&(rec.mat), ray, rec, &attenuation, &scattered, seed);
      else if (rec.mat.type == 1)
        did_scatter = scatter_metal(&(rec.mat), ray, rec, &attenuation, &scattered);

      if (did_scatter)
      {
        current_attenuation *= attenuation;
        ray = scattered;
      }
      else
        return ((float3)(0.0f));
    }
    else
    {
      //BACKGROUND
      float3 unit_dir;
      float3 white;
      float3 blue;
      float  a;

      unit_dir = normalize(ray.dir);
      a = 0.5f * (unit_dir.y + 1.0f);
      white = (float3)(1.0f, 1.0f, 1.0f);
      blue = (float3)(0.5f, 0.7f, 1.0f);
      float3 background = (1.0f - a) * white + a * blue;
      return current_attenuation * background;
    }
  }
  return (float3)(0.0f);
}

float3  sample_square(uint *seed)
{
  return ((float3)(random_float(0.0f, 1.0f, seed) - 0.5f, random_float(0.0f, 1.0f, seed) - 0.5f, 0.0f));
}

t_ray get_ray(t_image *frame, int x, int y, uint *seed)
{
  float3  offset;
  float3  pixel_sample;
  float3  ray_origin;
  float3  ray_dir;

  offset = sample_square(seed);
  pixel_sample = (frame->pixel_delta_u * (x + offset.x)) + (frame->pixel_delta_v * (y + offset.y)
                  + frame->pixel00_loc);
  ray_origin = frame->origin;
  ray_dir = pixel_sample - ray_origin;
  return (make_ray(ray_origin,  ray_dir));
}

void  write_color(__global int *img_buffer, int id, float3 color)
{
  uchar ir;
  uchar ig;
  uchar ib;
  uchar ia;
  t_interval intensity;

  intensity = interval_init(0.0f, 0.999f);
  ir = (uchar)(256 * interval_clamp(&intensity, color.x));
  ig = (uchar)(256 * interval_clamp(&intensity, color.y));
  ib = (uchar)(256 * interval_clamp(&intensity, color.z));
  ia = 255;

  __global uchar4 *ptr = (__global uchar4 *)img_buffer;

  ptr[id] = (uchar4)(ir, ig, ib, ia);
}

__kernel  void render_kernel(__global int *img_buffer, __global t_sphere *spheres, t_image frame)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  if (x >= frame.width || y >= frame.height)
    return ;
  int id = y * frame.width + x;

  uint seed = (uint)id;
  pcg_hash(&seed);

  uint    sample;
  t_ray   r;
  float3  color;

  sample = 0;
  color = (float3)(0.0f);
  while (sample < frame.samples_per_pixel)
  {

    r  = get_ray(&frame, x, y, &seed);
    color =  color + get_ray_color(spheres, frame.max_depth,  r, &seed);
    sample++;
  }
  write_color(img_buffer, id, color * frame.pixel_samples_scale);
}
