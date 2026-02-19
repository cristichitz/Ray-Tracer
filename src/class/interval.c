#include "interval.h"

bool  contains(t_interval *self, float x)
{
  return (self->min <= x && x <= self->max);
}

bool  surrounds(t_interval *self, float x)
{
  return (self->min < x && x < self->max);
}

float clamp(t_interval *self, float x)
{
  if (x < self->min)
      return (self->min);
  if (x > self->max)
      return (self->max);
  return (x);
}

t_interval interval_init(float t_min, float t_max)
{
  t_interval i;

  i.min = t_min;
  i.max = t_max;
  i.size = t_max - t_min;
  i.contains = contains;
  i.surrounds = surrounds;
  i.clamp = clamp;
  return (i);
}
