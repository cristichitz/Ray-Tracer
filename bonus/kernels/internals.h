#ifndef INTERNALS_H
#define INTERNALS_H

#ifdef  __OPENCL_VERSION__
  typedef float3  cl_float3;
#endif

typedef struct s_interval {
  float min;
  float max;
  float size;
} t_interval;

typedef struct s_ray {
  cl_float3 origin;
  cl_float3 dir;
} t_ray;

typedef struct  s_material {
  cl_float3 albedo;
  int       type;
} t_material;

typedef struct s_hit_record {
  cl_float3  p;
  cl_float3  normal;
  float   t;
  float   u;
  float   v;
  bool    front_face;
  int     obj_type;
  t_material      mat;
} t_hit_record;

#endif
