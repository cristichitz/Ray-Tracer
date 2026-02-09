#include "minirt.h"

void setup_local_opencl() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char vendors_path[PATH_MAX];
        snprintf(vendors_path, sizeof(vendors_path), "%s/config/vendors", cwd);
        
        setenv("OCL_ICD_VENDORS", vendors_path, 1);
        printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
    }
}

void  game_loop(void *param)
{
  t_data *data = (t_data *)param;
  float speed = 0.5f;

  if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
    mlx_close_window(data->mlx);
  if (mlx_is_key_down(data->mlx, MLX_KEY_W)) data->cam_z -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_S)) data->cam_z += speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_A)) data->cam_x -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_D)) data->cam_x += speed;

  render_frame(data);
}

void cleanup(void *param)
{
    t_data *data = (t_data *)param;
    clean_gpu(data);
    
    printf("Cleaning up and exiting...\n");
}    
int main(int ac, char **av)
{
  t_data  data;
  float   aspect_ratio;

  parse_input()


  aspect_ratio = 16.0 / 9.0;
  data.width = 1920;
  data.height = (int)(data.width / aspect_ratio);
  if (data.height < 1)
    data.height = 1;

  data.cam_x = 0.0f;
  data.cam_y = 0.0f;
  data.cam_z = 0.0f; 
  data.frame_count = 0;
  gettimeofday(&data.last_time, NULL); // Set "start" time to now


  setup_local_opencl();
  
  // 1. MLX Boilerplate
  printf("1. Initializing MLX42...\n");
  data.mlx = mlx_init(data.width, data.height, "GPU RT", true);
  if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  printf("3. Creating Image...\n");
  data.img = mlx_new_image(data.mlx, data.width, data.height);
  if (!data.img) { mlx_close_window(data.mlx); puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1) {
    mlx_close_window(data.mlx);
    puts(mlx_strerror(mlx_errno));
    return(EXIT_FAILURE);
  }

  printf("4. Initializing GPU...\n");
  init_gpu(&data, "kernels/render.c");

  printf("4. Starting Loop...");
  mlx_loop_hook(data.mlx, game_loop, &data);
  mlx_close_hook(data.mlx, cleanup, &data);
  mlx_loop(data.mlx);

  mlx_terminate(data.mlx);

  return (EXIT_SUCCESS);
}