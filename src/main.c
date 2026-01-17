#include "minirt.h"

// void  my_mlx_pixel_put(t_data *data, int x, int y, int color)
// {
//   char *dst;

//   dst = data->addr + (y * data->line_len + x * (data->bpp / 8));
//   *(unsigned int*)dst = color;
// }


// int close_window(int keycode, void *param)
// {
//   (void)param;
//   (void)keycode;
//   exit (0);
// }

// int main(void)
// {
//   void    *mlx;
//   void    *win;
//   t_data  img;
//   int     x;
//   int     y;
//   t_data  data;

//   float aspect_ratio = 16.0 / 9.0;
//   int   image_width = 2000;
//   int   image_height = (int)(image_width / aspect_ratio);
//   image_height = (image_height < 1) ? 1 : image_height;

//   float viewport_heigth = 2.0;
//   float viewport_width = viewport_heigth * ((float)image_width / (float)image_height);
//   (void)viewport_width;

//   mlx = mlx_init();
//   if (!mlx) 
//     return (1);
//   win = mlx_new_window(mlx, image_width, image_height, "MiniRT Test");
//   img.img = mlx_new_image(mlx, image_width, image_height);

//   img.addr = mlx_get_data_addr(img.img, &img.bpp, &img.line_len, &img.endian);
//   y = 0;
//   while (y < image_height)
//   {
//     x = 0;
//     while (x < image_width)
//     {
//       t_color pixel_col = vec3((double)x / (image_width - 1),
//                                 (double)y / (image_height - 1),
//                               0.25);
//       int mlx_color = color_to_int(pixel_col);
//       my_mlx_pixel_put(&img, x, y, mlx_color);
//       x++;
//     }
//     y++;
//   }
//   struct timeval current_time;
//   gettimeofday(&current_time, NULL);
//   if (data.last_time.tv_sec == 0) {
//       data.last_time = current_time;
//       return (0); // Skip first frame to avoid huge delta_time
//   }
//   long seconds = current_time.tv_sec - data.last_time.tv_sec;
//   long micros = current_time.tv_usec - data.last_time.tv_usec;
//   data.delta_time = seconds + micros * 1e-6;

//   data.last_time = current_time;
//   data.frame_count++;
//   if (data.delta_time > 0.001 && data.frame_count % 20 == 0) {
//   printf("FPS: %.0f\n", 1.0 / data.delta_time);  
//       fflush(stdout); // Force print immediately
//   }
//   mlx_put_image_to_window(mlx, win, img.img, 0, 0);
//   mlx_hook(win, 17, 0, close_window, NULL);
//   mlx_loop(mlx);
//   return (0);
// }

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_ESC 65307

int key_hook(int keycode, t_data *data)
{
    float speed = 0.5f; // Adjust move speed

    if (keycode == KEY_ESC)
        exit(0); // Clean exit

    if (keycode == KEY_W) data->cam_z -= speed; // Move Forward (into screen)
    if (keycode == KEY_S) data->cam_z += speed; // Move Back
    if (keycode == KEY_A) data->cam_x -= speed; // Move Left
    if (keycode == KEY_D) data->cam_x += speed; // Move Right
    
    // Optional: Up/Down
    // if (keycode == KEY_SPACE) data->cam_y += speed;
    // if (keycode == KEY_SHIFT) data->cam_y -= speed;

    // Re-render immediately after moving
    render_frame(data);
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    
    return (0);
}

int close_app(t_data *data)
{
  clean_gpu(data);
  exit(0);
  return (0);
}

#include <limits.h>
#include <unistd.h>
void setup_local_opencl() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char vendors_path[PATH_MAX];
        // Construct path: /current/path/config/vendors
        snprintf(vendors_path, sizeof(vendors_path), "%s/config/vendors", cwd);
        
        // Overwrite OCL_ICD_VENDORS to point to your local folder
        setenv("OCL_ICD_VENDORS", vendors_path, 1);
        
        // Optional: Debug print
        printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
    }
}

int main(void)
{
  t_data  data;
  float   aspect_ratio;

  aspect_ratio = 16.0 / 9.0;
  data.width = 3840;
  data.height = (int)(data.width / aspect_ratio);
  data.height = (data.height < 1) ? 1 : data.height;

  data.render_buffer = malloc(data.width * data.height * 4);
  if (!data.render_buffer) exit(1);

  data.cam_x = 0.0f;
  data.cam_y = 0.0f;
  data.cam_z = 0.0f; 


  data.frame_count = 0;
  gettimeofday(&data.last_time, NULL); // Set "start" time to now


  setup_local_opencl();
  // 1. MLX Boilerplate
  printf("1. Initializing Minilibx...\n");
  data.mlx = mlx_init();
  if (!data.mlx) { printf("MLX Init failed!\n"); return (1); }

  printf("2. Creating Window...\n");
  data.win = mlx_new_window(data.mlx, data.width, data.height, "GPU RT");

  printf("3. Creating Image...\n");
  data.img = mlx_new_image(data.mlx, data.width, data.height);
  data.addr = mlx_get_data_addr(data.img, &data.bpp, &data.line_len, &data.endian);

  printf("4. Initializing GPU...\n");
  init_gpu(&data, "kernels/render.c");

  printf("5. Rendering Frame...\n");
  render_frame(&data);

  printf("6. Displaying Image...\n");
  mlx_put_image_to_window(data.mlx, data.win, data.img, 0, 0);

  printf("7. Starting Loop...\n");
  mlx_hook(data.win, 17, 0, close_app, &data);
  mlx_loop_hook(data.mlx, (void *)render_frame, &data);
  mlx_key_hook(data.win, key_hook, &data);
  mlx_loop(data.mlx);

  return (0);
}