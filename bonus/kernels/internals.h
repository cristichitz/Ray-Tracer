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

/*
** albedo + type are the only fields the render kernel reads (0 = lambertian,
** 1 = metal, 2 = diffuse_light). The trailing fields are host-only physics
** properties (the device never touches them); a material therefore dictates
** both how an object looks AND how it behaves as a rigid body. dynamic = 1
** turns any object carrying this material into a simulated rigid body.
*/
typedef struct  s_material {
  cl_float3 albedo;
  int       type;
  int       dynamic;
  float     density;
  float     restitution;
  float     friction;
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
