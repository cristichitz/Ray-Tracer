#ifndef INTERVAL_H
#define INTERVAL_H

typedef struct s_interval {
  float min;
  float max;
  float size;
  bool  (*contains)(struct s_interval *self, float x);
  bool  (*surrounds)(struct s_interval *self, float x);
} t_interval;

bool  contains(t_interval *self, float x);
bool  surrounds(t_interval *self, float x);
#endif
