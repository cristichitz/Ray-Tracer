#include "init_kernel.h"

__kernel void render_kernel(__global int *img_buffer, __global t_object *objects,
                            int obj_count, t_image frame,
                            __global float4 *accum, int frame_index)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  if (x >= frame.width || y >= frame.height)
    return ;
  int id = y * frame.width + x;

  // Vary the seed per accumulated frame so each frame draws new samples.
  uint seed = (uint)id + (uint)frame_index * 9781u;
  pcg_hash(&seed);

  float3 color = (float3)(0.0f);
  uint   sample = 0;
  while (sample < frame.samples_per_pixel)
  {
    t_ray r = get_ray(&frame, x, y, &seed);
    color += get_ray_color(objects, obj_count, frame.max_depth, r, frame.background, &seed);
    sample++;
  }
  color *= frame.pixel_samples_scale;

  // Progressive accumulation: frame 0 overwrites, later frames add. The
  // displayed image is the running average over all accumulated frames.
  float3 acc;
  if (frame_index == 0)
    acc = color;
  else
    acc = accum[id].xyz + color;
  accum[id] = (float4)(acc, 1.0f);
  write_color(img_buffer, id, acc / (float)(frame_index + 1));
}
