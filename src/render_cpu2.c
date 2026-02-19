#include "rt_cpu.h"

t_vec3 get_ray_color(t_hittable_list world, t_ray ray)
{
  //We define a nice sphere;
  t_hit_record  hit_record;
  hit_record.set_face_normal = ft_set_face_normal;

  t_interval t_int;
  if (world.hit(&world, ray, interval_init(&t_int, 0, INFINITY), &hit_record))
    return scale(add(hit_record.normal, make_vec(1.0f, 1.0f, 1.0f)), 0.5f);

  //Background 

  // We normalize the direction vector meaning it's length is 1
  t_vec3 unit_dir = norm(ray.dir);
  
  // make it between 0 and 1 rather than -1 to 1
  float t = 0.5f * (unit_dir.y + 1.0f);
  t_vec3 white = make_vec(1.0f, 1.0f, 1.0f);
  t_vec3 blue = make_vec(0.5f, 0.7f, 1.0f);

  // (1 - t)white + t(blue);
  return add(scale(white, 1.0f - t), scale(blue, t));
}

int render_frame(t_data* data)
{
  for (uint32_t y = 0; y < data->height; y++)
  {
    for (uint32_t x = 0; x < data->width; x++) 
    {
      //t_vec3 pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
      t_vec3 pixel_center = add(data->pixel00_loc, scale(data->pixel_width, x));
      pixel_center = add(pixel_center, scale(data->pixel_height, y));
      t_vec3 direction = sub(pixel_center, data->origin);

      t_ray r = make_ray(data->origin, direction);

      t_vec3 color = get_ray_color(data->world, r);

      uint32_t ir = (uint32_t)(255.99f * color.x);
      uint32_t ig = (uint32_t)(255.99f * color.y);
      uint32_t ib = (uint32_t)(255.99f * color.z);
      uint32_t ia = 255;

      uint32_t pixel_color = (ir << 24) | (ig << 16) | (ib << 8) | ia;
      mlx_put_pixel(data->img, x, y, pixel_color);
    }
    
  }
  return (0);
}
