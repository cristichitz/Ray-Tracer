#ifndef INTERVAL_H
#define INTERVAL_H
#include <stdbool.h>

typedef struct s_interval {
  float min;
  float max;
  float size;
  bool  (*contains)(struct s_interval *self, float x);
  bool  (*surrounds)(struct s_interval *self, float x);
} t_interval;

bool        contains(t_interval *self, float x);
bool        surrounds(t_interval *self, float x);
t_interval  interval_init(t_interval *i, float min, float max);
#endif
