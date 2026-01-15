#ifndef MINIRT_H
# define MINIRT_H
#include "mlx.h"

#include "vec3.h"

#include "stdio.h"
#include "math.h"
#include <stdlib.h>

typedef struct s_data {
  void  *img;
  char  *addr;
  int   bits_per_pixel;
  int   line_length;
  int   endian;
} t_data;

void  my_mlx_pixel_put(t_data *data, int x, int y, int color);
int   close_window(int keycode, void *param);

#endif