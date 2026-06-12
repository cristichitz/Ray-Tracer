#include "init_kernel.h"

float3 get_ray_color(__global t_object *objs, int count, int max_depth,
                     t_ray ray, float3 background, uint *seed)
{
  t_hit_record  rec;
  t_ray         scattered;
  float3        attenuation;
  float3        throughput = (float3)(1.0f);
  float3        out = (float3)(0.0f);
  bool          count_emission = true;

  for (int depth = 0; depth < max_depth; depth++)
  {
    if (!hit_objects(objs, count, ray, interval_init(0.001f, INFINITY), &rec))
    {
      out += throughput * background;
      break ;
    }
    // Quad lights are handled by NEE, so only count their emission on the
    // camera ray / after a specular bounce. Everything else (sphere lights)
    // is still counted directly.
    if (count_emission || rec.obj_type != OBJ_QUAD || rec.mat.type != 2)
      out += throughput * emitted(&rec.mat);
    if (rec.mat.type == 1)
    {
      scatter_metal(&rec.mat, ray, rec, &attenuation, &scattered);
      count_emission = true;
    }
    else if (rec.mat.type == 0)
    {
      out += throughput * rec.mat.albedo * direct_light(objs, count, rec, seed);
      scatter_lambertian(&rec.mat, rec, &attenuation, &scattered, seed);
      count_emission = false;
    }
    else
      break ;
    throughput *= attenuation;
    ray = scattered;
    if (depth > 3)
    {
      float q = fmax(throughput.x, fmax(throughput.y, throughput.z));
      if (random_float(0.0f, 1.0f, seed) > q)
        break ;
      throughput /= q;
    }
  }
  return (out);
}

void  write_color(__global int *img_buffer, int id, float3 color)
{
  t_interval intensity = interval_init(0.0f, 0.999f);
  uchar ir = (uchar)(256 * interval_clamp(&intensity, color.x));
  uchar ig = (uchar)(256 * interval_clamp(&intensity, color.y));
  uchar ib = (uchar)(256 * interval_clamp(&intensity, color.z));
  uchar ia = 255;
  __global uchar4 *ptr = (__global uchar4 *)img_buffer;

  ptr[id] = (uchar4)(ir, ig, ib, ia);
}
