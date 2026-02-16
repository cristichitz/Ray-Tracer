#include "rt_cpu.h"


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

  render_frame(data);
}

int main(void)
{
    t_data  data;

    data.width = 1920;
    data.height = (int)(data.width / (16.0 / 9.0));
    data.height = (data.height < 1) ? 1 : data.height;

    data.cam_x = 0.0f;
    data.cam_y = 0.0f;
    data.cam_z = 0.0f;
    
    // Init MlX42
    data.mlx = mlx_init(data.width, data.height, "CPU RT", true);
    if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

    printf("2. Creating Image...\n");
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
