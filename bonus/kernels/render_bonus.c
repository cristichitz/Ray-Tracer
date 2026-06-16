#include "init_kernel.h"

__kernel void render_kernel(__global int *img_buffer, __constant t_object *objects,
                            int obj_count, t_image frame,
                            __global float4 *accum, int frame_index,
                            __global const t_bvh_node *nodes,
                            __global const int *prim,
                            int nnodes, int plane_first, int plane_count)
{
  int x = get_global_id(0);
  int y = get_global_id(1);

  if (x >= frame.width || y >= frame.height)
    return ;
  int id = y * frame.width + x;

  t_scene sc;
  sc.objs = objects;
  sc.nodes = nodes;
  sc.prim = prim;
  sc.count = obj_count;
  sc.nnodes = nnodes;
  sc.plane_first = plane_first;
  sc.plane_count = plane_count;

  // Vary the seed per accumulated frame so each frame draws new samples.
  uint seed = (uint)id + (uint)frame_index * 9781u;
  pcg_hash(&seed);

  float3 color = (float3)(0.0f);
  uint   sample = 0;
  while (sample < frame.samples_per_pixel)
  {
    t_ray r = get_ray(&frame, x, y, &seed);
    float3 s = get_ray_color(sc, frame.max_depth, r, frame.background, &seed);
    // Firefly clamp: a single extreme sample (lucky bounce into a bright
    // emitter) would dominate the running average for hundreds of frames.
    color += fmin(s, (float3)(10.0f));
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
