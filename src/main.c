#include "minirt.h"

void  my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
  char *dst;

  dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
  *(unsigned int*)dst = color;
}


int close_window(int keycode, void *param)
{
  (void)param;
  (void)keycode;
  exit (0);
}

int main(void)
{
  void    *mlx;
  void    *win;
  t_data  img;
  int     x;
  int     y;

  float aspect_ratio = 16.0 / 9.0;
  int   image_width = 400;
  int   image_height = (int)(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  float viewport_heigth = 2.0;
  float viewport_width = viewport_heigth * ((float)image_width / (float)image_height);

  mlx = mlx_init();
  if (!mlx) 
    return (1);
  win = mlx_new_window(mlx, image_width, image_height, "MiniRT Test");
  img.img = mlx_new_image(mlx, image_width, image_height);

  img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
  y = 0;
  while (y < image_height)
  {
    x = 0;
    while (x < image_width)
    {
      t_color pixel_col = vec3((double)x / (image_width - 1),
                                (double)y / (image_height - 1),
                              0.25);
      int mlx_color = color_to_int(pixel_col);
      my_mlx_pixel_put(&img, x, y, mlx_color);
      x++;
    }
    y++;
  }
  mlx_put_image_to_window(mlx, win, img.img, 0, 0);
  mlx_hook(win, 17, 0, close_window, NULL);
  mlx_loop(mlx);
  return (0);
}