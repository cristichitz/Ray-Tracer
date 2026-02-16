#include "rt_cpu.h"

t_vec3 add(t_vec3 a, t_vec3 b)
{
  return (t_vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

t_vec3 sub(t_vec3 a, t_vec3 b)
{
  return (t_vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

t_vec3 scale(t_vec3 a, float b)
{
  return (t_vec3){a.x * b, a.y * b, a.z * b};
}

float dot(t_vec3 a, t_vec3 b)
{
  return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vec3 norm(t_vec3 a)
{
  float len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
  return scale(a, sq);
}


t_vec3 make_vec(float a, float b, float c)
{
  return (t_vec3){a, b, c};
}

void  print_vec(t_vec3 a)
{
  printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

t_vec3 get_ray_color(t_vec3 origin, t_vec3 dir)
{
  t_vec3 sphere_center = make_vec(0.0f, 0.0f, -1.0f);

  float radius = 0.5f;

  t_vec3 oc = sub(origin, sphere_center);

  float a = dot(dir, dir);
  float b = 2.0f * dot(oc, dir);

  float c = dot(oc, oc) - radius * radius;

  float discriminant = b*b - 4.0f * a * c;

  if (discriminant > 0.0f)
  {
    float t = (-b - sqrtf(discriminant)) / (2.0f * a);
    t_vec3 hit_point = add(origin, scale(dir, t));
    t_vec3 normal = norm(sub(hit_point, sphere_center));
    return scale(add(normal, make_vec(1.0f, 1.0f, 1.0f)), 0.5f);
  }
  else
  {
    t_vec3 unit_dir = norm(dir);
    float t = 0.5f * (unit_dir.y + 1.0f);
    t_vec3 white = make_vec(1.0f, 1.0f, 1.0f);
    t_vec3 blue = make_vec(0.5f, 0.7f, 1.0f);

    return add(scale(white, 1.0f - t), scale(blue, t));
  }
  
}

int render_frame(t_data* data) {
  float aspect_ratio = (float)data->width / (float)data->height;
  float viewport_height = 2.0f;
  float viewport_width = aspect_ratio * viewport_height;
  float focal_length = 1.0f;

  t_vec3 origin = make_vec(data->cam_x, data->cam_y, data->cam_z);
  t_vec3 horizontal = make_vec(viewport_width, 0.0f, 0.0f);
  t_vec3 vertical = make_vec(0.0f, viewport_height, 0.0f);
  

  t_vec3 lower_left_corner = sub(origin, scale(horizontal, 0.5f));
  lower_left_corner = sub(lower_left_corner, scale(vertical, 0.5f));
  lower_left_corner = sub(lower_left_corner, make_vec(0.0f, 0.0f, focal_length));

  for (uint32_t y = 0; y < data->height; y++)
  {
    for (uint32_t x = 0; x < data->width; x++) 
    {
      float u = (float)x / (float)(data->width - 1);
      float v = 1.0f - ((float)y / (data->height - 1));

      t_vec3 direction = add(lower_left_corner, scale(horizontal, u));
      direction = add(direction, scale(vertical, v));
      direction = sub(direction, origin);

      t_vec3 color = get_ray_color(origin, direction);


      uint32_t ir = (uint32_t)(255.99f * color.x);
      uint32_t ig = (uint32_t)(255.99f * color.y);
      uint32_t ib = (uint32_t)(255.99f * color.z);
      uint32_t ia = 255;

      uint32_t pixel_color = (ir << 24) | (ig << 16) | (ib << 8) | ia;
      mlx_put_pixel(data->img, x, y, pixel_color);
    }
    
  }
  //mlx_put_pixel(data->img, x, y, pixel_color);
  return (0);
}
