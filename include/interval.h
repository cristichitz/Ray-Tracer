#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdbool.h>

#include <stdio.h>
typedef struct s_interval {
  float min;
  float max;
  float size;
  bool  (*contains)(struct s_interval *self, float x);
  bool  (*surrounds)(struct s_interval *self, float x);
  float (*clamp)(struct s_interval *self, float x);
} t_interval;

t_interval  interval_init(float min, float max);
#endif
