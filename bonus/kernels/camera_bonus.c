#include "init_kernel.h"

float3  sample_square(uint *seed)
{
  return ((float3)(random_float(0.0f, 1.0f, seed) - 0.5f,
                   random_float(0.0f, 1.0f, seed) - 0.5f, 0.0f));
}

t_ray get_ray(t_image *frame, int x, int y, uint *seed)
{
  float3 offset = sample_square(seed);
  float3 pixel_sample = (frame->pixel_delta_u * (x + offset.x))
                        + (frame->pixel_delta_v * (y + offset.y) + frame->pixel00_loc);
  float3 ray_origin = frame->origin;
  float3 ray_dir = pixel_sample - ray_origin;

  return (make_ray(ray_origin, ray_dir));
}
