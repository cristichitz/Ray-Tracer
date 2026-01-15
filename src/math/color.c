#include "vec3.h"

int color_to_int(t_color pixel_color)
{
  int r = (int)(255.999 * pixel_color.x);
  int g = (int)(255.999 * pixel_color.y);
  int b = (int)(255.999 * pixel_color.z);

  return ((r << 16) | (g << 8) | b);
}