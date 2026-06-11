#ifndef INIT_KERNEL_H
#define INIT_KERNEL_H


#ifdef  __OPENCL_VERSION__
  typedef float3  cl_float3;
#endif

#include "internals.h"

#define RAND_MAX 2147483647

/*
** One tagged struct for every primitive. The render kernel branches on `type`
** instead of using function pointers (unavailable in OpenCL 1.2). The host
** fills a flat array of these and uploads it as a single buffer, so the scene
** is driven entirely by the parser/terminal with no kernel changes needed.
*/
#define OBJ_SPHERE   0
#define OBJ_PLANE    1
#define OBJ_CYLINDER 2
#define OBJ_QUAD     3

typedef struct s_object {
  int        type;
  cl_float3  center;   // sphere center / plane,quad Q / cylinder midpoint
  cl_float3  normal;   // plane, cylinder axis, quad normal
  cl_float3  u;        // quad edge 1
  cl_float3  v;        // quad edge 2
  cl_float3  w;        // quad: n / dot(n, n)
  float      radius;   // sphere, cylinder
  float      height;   // cylinder
  float      d;        // plane/quad: dot(normal, Q)
  float      d_top;    // cylinder top cap (reserved)
  t_material material;
} t_object;

typedef struct s_image
{
  int  width;
  int  height;

  float aspect_ratio;
  float viewport_height;
  float viewport_width;
  float focal_length;

  cl_float3 horizontal;
  cl_float3 vertical;

  int       samples_per_pixel;
  float     pixel_samples_scale;
  int       max_depth;

  // PREVIOUSLY T_CAMERA STRUCT
  cl_float3   origin;
  cl_float3   pixel00_loc;
  cl_float3   pixel_delta_u;
  cl_float3   pixel_delta_v;

  // Flat colour returned when a ray hits nothing (ambient sky / Cornell black)
  cl_float3   background;
} t_image;

#endif
