#ifndef INTERVAL_BONUS_H
#define INTERVAL_BONUS_H
#include <stdbool.h>

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
