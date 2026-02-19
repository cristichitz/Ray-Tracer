#include "rt_cpu.h"

t_vec3 get_ray_color(t_hittable_list world, t_ray ray)
{
  //We define a nice sphere;
  t_hit_record  hit_record;
  hit_record.set_face_normal = ft_set_face_normal;
  /* bool hit = data->sphere.hit(&data->sphere, ray, 0, INFINITY, &hit_record); */

  if (world.hit(&world, ray, 0, INFINITY, &hit_record))
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

  t_vec3 origin = make_vec(data->cam_x, data->cam_y, data->cam_z);
  t_vec3 horizontal = make_vec(data->viewport_width, 0.0f, 0.0f);
  t_vec3 vertical = make_vec(0.0f, -data->viewport_height, 0.0f);

  t_vec3 pixel_delta_u =  scale(horizontal, (float)1 / (float)data->width);
  t_vec3 pixel_delta_v = scale(vertical, (float)1 / (float)data->height);
  
  //upper = origin - horizontal / 2 - vertical / 2 - focal_length
  t_vec3 upper_left_corner = sub(origin, scale(horizontal, 0.5f));
  upper_left_corner = sub(upper_left_corner, scale(vertical, 0.5f));
  upper_left_corner = sub(upper_left_corner, make_vec(0.0f, 0.0f, data->focal_length));

  // corner + 0.5 * (pixel_delta_u + pixel_delta_v)
  t_vec3 pixel00_loc = add(upper_left_corner, scale(add(pixel_delta_u, pixel_delta_v), 0.5f)); 

  for (uint32_t y = 0; y < data->height; y++)
  {
    for (uint32_t x = 0; x < data->width; x++) 
    {

      //t_vec3 pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
      t_vec3 pixel_center = add(pixel00_loc, scale(pixel_delta_u, x));
      pixel_center = add(pixel_center, scale(pixel_delta_v, y));
      t_vec3 direction = sub(pixel_center, origin);

      t_ray r = make_ray(origin, direction);

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
