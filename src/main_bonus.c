#include "rt_bonus.h"

// SPACE scrambles, ENTER solves. Edge-triggered via the key hook so one press
// queues exactly one batch of moves (mlx_is_key_down would fire every frame).
void  key_hook(mlx_key_data_t key, void *param)
{
  t_data  *data;

  data = (t_data *)param;
  if (key.action != MLX_PRESS)
    return ;
  if (data->render_mode || data->phys.running)
    return ;
  if (key.key == MLX_KEY_SPACE)
    scramble_rubik(&data->rubik);
  else if (key.key == MLX_KEY_ENTER)
    solve_rubik(&data->rubik);
  else if (key.key == MLX_KEY_R)
    start_explode(data);
}

void game_loop(void *param)
{
  t_data  *data = (t_data *)param;
  float   speed = 10.5f;
  float   rot_speed = 0.05f;
  int     moved = 0;

  if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
  {
    cleanup(data);
    mlx_close_window(data->mlx);
    return ;
  }

  if (mlx_is_key_down(data->mlx, MLX_KEY_LEFT))  { data->cam_yaw -= rot_speed; moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_RIGHT)) { data->cam_yaw += rot_speed; moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_UP))    { data->cam_pitch += rot_speed; moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_DOWN))  { data->cam_pitch -= rot_speed; moved = 1; }

  if (data->cam_pitch > (float)CL_M_PI / 2.0f - 0.01f) data->cam_pitch = (float)CL_M_PI / 2.0f - 0.01f;
  if (data->cam_pitch < (float)-CL_M_PI / 2.0f + 0.01f) data->cam_pitch = (float)-CL_M_PI / 2.0f + 0.01f;

  cl_float3 forward = data->cam_dir;
  cl_float3 right = norm(cross(make_float3(0.0f, 1.0f, 0.0f), forward));
  cl_float3 up = norm(cross(forward, right));
  cl_float3 step = make_float3(0.0f, 0.0f, 0.0f);

  if (mlx_is_key_down(data->mlx, MLX_KEY_W)) { step = add(step, scale(forward, speed)); moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_S)) { step = add(step, scale(forward, -speed)); moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_A)) { step = add(step, scale(right, -speed)); moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_D)) { step = add(step, scale(right, speed)); moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_E)) { step = add(step, scale(up, speed)); moved = 1; }
  if (mlx_is_key_down(data->mlx, MLX_KEY_Q)) { step = add(step, scale(up, -speed)); moved = 1; }

  if (moved || data->rubik.active || data->rubik.explode_active
    || data->rubik.orbit_active || data->phys.running)
  {
    if (moved)
      data->cam_center = add(data->cam_center, step);
    data->frame_index = 0; // Restart path tracing accumulations!
    update_view(data);
  }

  step_rubik(data);
  physics_step(data);
  update_stage(data);

  render_frame(data);
}

int main(int argc, char **argv)
{
  t_data  data;

  SetupLocalCL();
  memset(&data, 0, sizeof(data));
  data.render_mode = render_mode_on(argc, argv);
  if (!load_scene(&data, argc, argv))
    return (EXIT_FAILURE);
  initialize(&data);

  data.mlx = mlx_init(data.frame.width, data.frame.height, "GPU RT", true);
  if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return (EXIT_FAILURE); }
  data.img = mlx_new_image(data.mlx, data.frame.width, data.frame.height);
  if (!data.img) { mlx_close_window(data.mlx); puts(mlx_strerror(mlx_errno)); return (EXIT_FAILURE); }
  if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1) {
    mlx_close_window(data.mlx);
    puts(mlx_strerror(mlx_errno));
    return (EXIT_FAILURE);
  }

  init_gpu(&data);
  mlx_loop_hook(data.mlx, data.render_mode ? render_loop : game_loop, &data);
  mlx_key_hook(data.mlx, key_hook, &data);
  mlx_close_hook(data.mlx, cleanup, &data);
  mlx_loop(data.mlx);

  mlx_terminate(data.mlx);
  return (EXIT_SUCCESS);
}
