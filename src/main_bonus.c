#include "rt_bonus.h"

// SPACE scrambles, ENTER solves. Edge-triggered via the key hook so one press
// queues exactly one batch of moves (mlx_is_key_down would fire every frame).
void  key_hook(mlx_key_data_t key, void *param)
{
  t_data  *data;

  data = (t_data *)param;
  if (key.action != MLX_PRESS)
    return ;
  if (key.key == MLX_KEY_SPACE)
    scramble_rubik(&data->rubik);
  else if (key.key == MLX_KEY_ENTER)
    solve_rubik(&data->rubik);
}

void game_loop(void *param)
{
  t_data  *data = (t_data *)param;
  float   speed = 1.0f;

  if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
  {
    cleanup(data);
    mlx_close_window(data->mlx);
    return ;
  }
  if (mlx_is_key_down(data->mlx, MLX_KEY_W))
    data->cam_center.z += speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_S))
    data->cam_center.z -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_A))
    data->cam_center.x -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_D))
    data->cam_center.x += speed;
  update_view(data);
  step_rubik(data);
  render_frame(data);
}

int main(int argc, char **argv)
{
  t_data  data;

  SetupLocalCL();
  memset(&data, 0, sizeof(data));
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
  mlx_loop_hook(data.mlx, game_loop, &data);
  mlx_key_hook(data.mlx, key_hook, &data);
  mlx_close_hook(data.mlx, cleanup, &data);
  mlx_loop(data.mlx);

  mlx_terminate(data.mlx);
  return (EXIT_SUCCESS);
}
