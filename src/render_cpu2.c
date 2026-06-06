#include "rt_cpu.h"

t_vec3 get_ray_color(t_hittable_list world, int depth, t_ray ray)
{
  //We define a nice sphere;
  t_hit_record  hit_record;
  hit_record.set_face_normal = ft_set_face_normal;

  g_ray_count++;
  if (depth <= 0)
    return  make_vec(0.0f, 0.0f, 0.0f);
  
  if (!world.hit(&world, ray, interval_init(0.001f, INFINITY), &hit_record))
    return (world.background);
  
  t_ray   scattered;
  t_vec3  attenuation;
  t_vec3  color_from_emmision = hit_record.mat.emmited(&hit_record.mat, hit_record.u, hit_record.v, hit_record.p);
  t_vec3  color_from_ambient = mult(world.ambient, hit_record.mat.tex.albedo);

  color_from_emmision = add(color_from_emmision, color_from_ambient);
  if (!hit_record.mat.scatter(&hit_record.mat, ray, hit_record, &attenuation, &scattered))
    return (color_from_emmision);

  t_vec3 color_from_scatter = mult(attenuation, get_ray_color(world, depth - 1, scattered));

  return (add(color_from_emmision, color_from_scatter));

  // attenuation = make_vec(1.0f, 1.0f, 1.0f);

  // if (hit_record.mat.scatter(&(hit_record.mat), ray, hit_record, &attenuation, &scattered))
  //   return mult(attenuation, get_ray_color(world, depth - 1, scattered));
  // return make_vec(0.0f, 0.0f, 0.0f);

  // return (hit_record.colour);
  // return scale(add(hit_record.normal, make_vec(1.0f, 1.0f, 1.0f)), 0.5f);
  // (normal + 1.0f) * 0.5f

  //Background 

  // We normalize the direction vector meaning it's length is 1
  // t_vec3 unit_dir = norm(ray.dir);
  
  // // make it between 0 and 1 rather than -1 to 1
  // float t = 0.5f * (unit_dir.y + 1.0f);
  // t_vec3 white = make_vec(1.0f, 1.0f, 1.0f);
  // t_vec3 blue = make_vec(0.5f, 0.7f, 1.0f);

  // // (1 - t)white + t(blue);
  // return add(scale(white, 1.0f - t), scale(blue, t));
}

t_vec3  sample_square(void)
{
  return make_vec(random_float(0.0f, 1.0f) - 0.5, random_float(0.0f, 1.0f) - 0.5f, 0);
}

t_ray get_ray(t_data *data, uint32_t x, uint32_t y)
{
  // A point at most abs(0.5, 0.5, 0) away from the current point
  // We will average the colors around this point
  t_vec3 offset = sample_square();

  t_vec3 pixel_sample = scale(data->px_w, x + offset.x);
  pixel_sample = add(pixel_sample, scale(data->px_h, y + offset.y));
  pixel_sample = add(pixel_sample, data->pixel00_loc);

  // px_sample = (px_w * (x + off.x)) + (ph_h * (y + off.y)) + pixel00_loc;

  t_vec3 ray_origin = data->origin;
  t_vec3 ray_direction = sub(pixel_sample, ray_origin);

  return make_ray(ray_origin, ray_direction);
}

void  write_color(t_data *data, uint32_t x, uint32_t y, t_vec3 color)
{
  uint32_t r;
  uint32_t g;
  uint32_t b;
  uint32_t pixel_color;
  t_interval intensity;

  intensity = interval_init(0.0f, 0.999f);

  r = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.x)));
  g = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.y)));
  b = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.z)));
  pixel_color = (r <<  24) | (g << 16) | (b << 8) | 255;
  if (data->headless)
    data->fb[(size_t)y * data->width + x] = pixel_color;
  else
    mlx_put_pixel(data->img, x, y, pixel_color);
}

int render_frame(t_data* data)
{
  /* t_vec3  pixel_center; */
  /* t_vec3  direction; */
  t_ray   r;
  t_vec3  color;

  for (uint32_t y = 0; y < data->height; y++)
  {
    for (uint32_t x = 0; x < data->width; x++) 
    {
      //t_vec3 pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
      /* pixel_center = add(data->pixel00_loc, scale(data->pixel_width, x)); */
      /* pixel_center = add(pixel_center, scale(data->pixel_height, y)); */
      /* direction = sub(pixel_center, data->origin); */
      /* r = make_ray(data->origin, direction); */
      /* color = get_ray_color(data->world, r); */
      /* write_color(data, x, y, color); */

      // NEW
      color = make_vec(0.0f, 0.0f, 0.0f);
      for (uint32_t sample = 0; sample < data->samples_per_pixel; sample++)
      {

        r = get_ray(data, x, y);
        color = add(color, get_ray_color(data->world, data->max_depth, r));

      }
      write_color(data, x, y, scale(color, data->pixel_samples_scale));
    }
    
  }
  return (0);
}
