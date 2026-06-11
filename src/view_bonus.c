#include "rt_bonus.h"

/*
** Recompute the per-frame viewport origin / top-left pixel from the current
** camera position. Mirrors update_viewport() in the CPU build. Called once at
** startup and again whenever the camera moves in game_loop.
*/
void  update_view(t_data *data)
{
  cl_float3 vup;
  cl_float3 w;
  cl_float3 u;
  cl_float3 v;
  cl_float3 ulc;

  data->cam_dir.x = sinf(data->cam_yaw) * cosf(data->cam_pitch);
  data->cam_dir.y = sinf(data->cam_pitch);
  data->cam_dir.z = cosf(data->cam_yaw) * cosf(data->cam_pitch);
  data->cam_dir = norm(data->cam_dir);

  vup = make_float3(0.0f, 1.0f, 0.0f);
  w = data->cam_dir;
  if (fabs(w.x) < 1e-5 && fabs(w.z) < 1e-5)
    vup = make_float3(0.0f, 0.0f, 1.0f);
  u = norm(cross(vup, w));
  v = cross(w, u);

  data->frame.horizontal = scale(u, data->frame.viewport_width);
  data->frame.vertical = scale(v, -data->frame.viewport_height);
  data->frame.pixel_delta_u = scale(data->frame.horizontal, 1.0f / (float)data->frame.width);
  data->frame.pixel_delta_v = scale(data->frame.vertical, 1.0f / (float)data->frame.height);

  data->frame.origin = data->cam_center;
  ulc = sub(data->frame.origin, scale(data->frame.horizontal, 0.5f));
  ulc = sub(ulc, scale(data->frame.vertical, 0.5f));
  ulc = add(ulc, scale(w, data->frame.focal_length));
  data->frame.pixel00_loc = add(ulc, scale(add(data->frame.pixel_delta_u,
          data->frame.pixel_delta_v), 0.5f));
}

/*
** Build the camera basis (u, v, w) from cam_dir + fov and derive the viewport
** vectors. Ported from initialize() in src/main_cpu.c.
*/
void  initialize(t_data *data)
{
  cl_float3 dir;

  data->frame.width = BONUS_WIDTH;
  data->frame.height = (int)(data->frame.width / (16.0 / 9.0));
  if (data->frame.height < 1)
    data->frame.height = 1;
  data->frame.aspect_ratio = (float)data->frame.width / (float)data->frame.height;
  data->frame.viewport_height = 2.0f * tanf(degrees_to_radians(data->cam_fov) / 2.0f);
  data->frame.viewport_width = data->frame.aspect_ratio * data->frame.viewport_height;
  data->frame.focal_length = 1.0f;
  data->frame.samples_per_pixel = BONUS_SPP;
  data->frame.pixel_samples_scale = 1.0f / data->frame.samples_per_pixel;
  data->frame.max_depth = BONUS_MAX_DEPTH;

  dir = norm(data->cam_dir);
  data->cam_yaw = atan2f(dir.x, dir.z);
  data->cam_pitch = asinf(fmaxf(-1.0f, fminf(1.0f, dir.y)));

  update_view(data);
}
