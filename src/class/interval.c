#include "interval.h"

bool  ft_contains(t_interval *self, float x)
{
  return (self->min <= x && x <= self->max);
}

bool  ft_surrounds(t_interval *self, float x)
{
  return (self->min < x && x < self->max);
}

t_interval interval_init(t_interval *i, float t_min, float t_max)
{
  i->min = t_min;
  i->max = t_max;
  i->size = t_max - t_min;
  i->contains = ft_contains;
  i->surrounds = ft_surrounds;
  return (*i);
}
