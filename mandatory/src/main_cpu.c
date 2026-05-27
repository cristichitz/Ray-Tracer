#include "rt_cpu.h"

float random_float(float min, float max)
{
  // random is a number between 0 and 1
  return (min + (max - min)*((rand() / ((double)RAND_MAX + 1))));
}

// Constants
float degrees_to_radians(float degrees)
{
  return (degrees * M_PI / 180.0f);
}

void game_loop(void *param)
{
  t_data *data = (t_data *)param;
  float speed = 1.0f;

  if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
    mlx_close_window(data->mlx);

  if (mlx_is_key_down(data->mlx, MLX_KEY_W)) data->cam_z -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_S)) data->cam_z += speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_A)) data->cam_x -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_D)) data->cam_x += speed;

  data->origin = make_vec(data->cam_x, data->cam_y, data->cam_z);
  render_frame(data);
}

void initialize(t_data *data)
{
    data->width = 1920;
    data->height = (int)(data->width / (16.0 / 9.0));
    data->height = (data->height < 1) ? 1 : data->height;

    data->cam_x = 0.0f;
    data->cam_y = 0.0f;
    data->cam_z = 0.0f;
    
    data->origin = make_vec(data->cam_x, data->cam_y, data->cam_z);

    data->aspect_ratio = (float)data->width / (float)data->height;
    data->viewport_height = 2.0f;
    data->viewport_width = data->aspect_ratio * data->viewport_height;
    data->focal_length = 1.0f;

    // New. For Antialising
    data->samples_per_pixel = 5;
    data->pixel_samples_scale = 1.0f / data->samples_per_pixel;
    data->max_depth = 2;
    // until here

    // This was in rendering but they are alwas  the same
    data->horizontal = make_vec(data->viewport_width, 0.0f, 0.0f);
    data->vertical = make_vec(0.0f, -data->viewport_height, 0.0f);

    data->pixel_width =  scale(data->horizontal, (float)1 / (float)data->width);
    data->pixel_height = scale(data->vertical, (float)1 / (float)data->height);

    t_vec3 upper_left_corner = sub(data->origin, scale(data->horizontal, 0.5f));
    upper_left_corner = sub(upper_left_corner, scale(data->vertical, 0.5f));
    upper_left_corner = sub(upper_left_corner, make_vec(0.0f, 0.0f, data->focal_length));

    // corner + 0.5 * (pixel_delta_u + pixel_delta_v)
    data->pixel00_loc = add(upper_left_corner, scale(add(data->pixel_width, data->pixel_height), 0.5f)); 
}

int create_objects(t_hittable_list *world)
{
  int status;
  t_material    material_ground;
  t_material    material_left;
  t_material    material_center;
  t_material    material_right;


  material_ground = init_lambertian(make_vec(0.8f, 0.8f, 0.0f));
  material_left = init_metal(make_vec(0.8f, 0.8f, 0.8f));
  material_center = init_metal(make_vec(0.1f, 0.2f, 0.5f));
  material_right = init_metal(make_vec(0.8f, 0.6f, 0.2f));


  status = world->add(world, make_sphere(make_vec(0, -100.5, -1.0), 100.0, material_ground));
  status = world->add(world, make_sphere(make_vec(0, 0, -1.2), 0.5, material_left));
  status = world->add(world, make_sphere(make_vec(-1, 0, -1), 0.5, material_center));
  status = world->add(world, make_sphere(make_vec(1, 0, -1), 0.5, material_right));
  status = world->add(world, make_sphere(make_vec(1, 1, -1), 0.5, material_right));

  return (status);
}

int main(void)
{
    t_data          data;
    t_hittable_list world;
    t_list          obj;

    initialize(&data);
    world.objects = &obj;
    if (init_world(&world))
      return (EXIT_FAILURE);
    if (create_objects(&world) == EXIT_FAILURE)
      return (EXIT_FAILURE);

    data.world = world;
    // Init MlX42
    data.mlx = mlx_init(data.width, data.height, "CPU RT", true);
    if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

    data.img = mlx_new_image(data.mlx, data.width, data.height);
    if (!data.img) { mlx_close_window(data.mlx); puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

    if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1) {
      mlx_close_window(data.mlx);
      puts(mlx_strerror(mlx_errno));
      return(EXIT_FAILURE);
    }
    mlx_loop_hook(data.mlx, game_loop, &data);
    mlx_loop(data.mlx);
    mlx_terminate(data.mlx);

    return (EXIT_SUCCESS);
}
