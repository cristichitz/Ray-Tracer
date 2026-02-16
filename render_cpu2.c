#include "rt_cpu.h"

t_vec3 get_ray_color(t_data *data, t_ray ray)
{
  //We define a nice sphere;
  float t = data->sphere.hit(&data->sphere, ray);

  if (t > 0.0f)
  {
    // origim + time * direction
    // P(t) = O + t*D;
    t_vec3 hit_point = ray.at(&ray, t);
    // Normal vector from sphere_center to hit_point
    t_vec3 normal = sub(hit_point, data->sphere.center);
    // unit vector of the normal
    t_vec3 unit_vec = norm(normal);
    // (unit_vec + 1) / 2 
    // the unit vector ranges from -1 to 1
    // we scale it to be between 0 and 1 and use its x y z values as r g b colors
    return scale(add(unit_vec, make_vec(1.0f, 1.0f, 1.0f)), 0.5f);
  }
  else
  {
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
}

t_vec3 ray_at(t_ray *self, float t)
{
  t_vec3 result;
  result = add(self->origin, scale(self->dir, t));
  return (result);
}

t_ray make_ray(t_vec3 origin, t_vec3 direction) {
  t_ray r;

  r.origin = origin;
  r.dir = direction;
  r.at = ray_at;
  return (r);
}

int render_frame(t_data* data) {

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

      t_vec3 color = get_ray_color(data, r);

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
