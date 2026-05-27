#include "rt_bonus.h"

t_vec3 get_ray_color(t_hittable_list world, int depth, t_ray ray)
{
  //We define a nice sphere;
  t_hit_record  rec;
  rec.set_face_normal = ft_set_face_normal;

  if (depth <= 0)
    return make_vec(0.0f, 0.0f, 0.0f);

  /* if (world.hit(&world, ray, interval_init(0, INFINITY), &rec)) */
  /*   return scale(add(hit_record.normal, make_vec(1.0f, 1.0f, 1.0f)), 0.5f); */
  //Before
  // 0.5 * (normal + vec(1)) = [0, 1] 

  if (world.hit(&world, ray, interval_init(0.001f, INFINITY), &rec))
  {
    t_ray   scattered;
    t_vec3  attenuation;
    
    if (rec.mat.scatter(&(rec.mat), ray, rec, &attenuation, &scattered))
      return mult(attenuation, get_ray_color(world, depth - 1, scattered));
    return make_vec(0.0f, 0.0f, 0.0f);
    /* t_vec3 direction = add(rec.normal, random_on_hemisphere(rec.normal)); */
    /* t_vec3 direction = random_on_hemisphere(rec.normal); */
    /* return scale(get_ray_color(world, depth - 1, make_ray(rec.p, direction)), 0.5f); */
  }

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

t_vec3  sample_square(void)
{
  return make_vec(random_float(0.0f, 1.0f) - 0.5, random_float(0.0f, 1.0f) - 0.5f, 0);
}

t_ray get_ray(t_data *data, uint32_t x, uint32_t y)
{
  // A point at most abs(0.5, 0.5, 0) away from the current point
  // We will average the colors around this point
  t_vec3 offset = sample_square();

  t_vec3 pixel_sample = scale(data->pixel_width, x + offset.x);
  pixel_sample = add(pixel_sample, scale(data->pixel_height, y + offset.y));
  pixel_sample = add(pixel_sample, data->pixel00_loc);

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

  r = (uint32_t)(256 * intensity.clamp(&intensity, color.x));
  g = (uint32_t)(256 * intensity.clamp(&intensity, color.y));
  b = (uint32_t)(256 * intensity.clamp(&intensity, color.z));
  pixel_color = (r <<  24) | (g << 16) | (b << 8) | 255;
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
      // WITHOUT PIXEL SAMPLING
      
      //t_vec3 pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
      /* pixel_center = add(data->pixel00_loc, scale(data->pixel_width, x)); */
      /* pixel_center = add(pixel_center, scale(data->pixel_height, y)); */
      /* direction = sub(pixel_center, data->origin); */
      /* r = make_ray(data->origin, direction); */
      /* color = get_ray_color(data->world, r); */
      /* write_color(data, x, y, color); */

      // NEW. PIXEL SAMPLING
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
