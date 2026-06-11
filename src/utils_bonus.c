#include "rt_bonus.h"

float random_float(float min, float max)
{
  // random is a number between 0 and 1
  return (min + (max - min) * ((rand() / ((double)RAND_MAX + 1))));
}

float degrees_to_radians(float degrees)
{
  return (degrees * M_PI / 180.0f);
}
