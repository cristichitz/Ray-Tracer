#include "interval.h"

bool  contains(t_interval *self, float x)
{
  return (self->min <= x && x <= self->max);
}

bool  surrounds(t_interval *self, float x)
{
  return (self->min < x && x < self->max);
}

void interval_init(t_interval *i, float min, float max)
{
  i->min = min;
  i->max = max;
  i->size = max - min;
  i->contains = contains;
  i->surrounds = surrounds;
}
