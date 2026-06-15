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
// Oriented ellipse/disk: like a quad but `center` is the true center and
// u, v are the (perpendicular) semi-axis vectors. Used as the portal surface.
#define OBJ_ELLIPSE  4

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

/*
** One flat BVH node, shared by the host builder and the kernel traversal.
** The tree covers every bounded primitive (planes are infinite, so they are
** kept in a separate linear list). Root is node 0.
*/
typedef struct s_bvh_node {
  cl_float3 bmin;
  cl_float3 bmax;
  int       left;   // inner: left child index / leaf: first slot in prim list
  int       right;  // inner: right child index (unused for leaves)
  int       count;  // 0 = inner node, > 0 = leaf primitive count
  int       pad;
} t_bvh_node;

#ifdef __OPENCL_VERSION__
/*
** Read-only views of the scene, bundled so every kernel helper takes one
** argument instead of five. Device-only: host code never sees this struct.
*/
typedef struct s_scene {
  __constant t_object   *objs;
  // nodes/prim live in __global: together with the objects they can outgrow
  // the 64KB constant budget once scenes pass a few hundred primitives.
  __global const t_bvh_node *nodes;
  __global const int        *prim;    // object indices; planes at the tail
  int                    count;       // total objects (NEE light scan)
  int                    nnodes;
  int                    plane_first; // == number of prims inside the BVH
  int                    plane_count;
  // Object indices of the two active portals (-1 = inactive). When a ray hits
  // an OBJ_ELLIPSE whose material.portal is 0/1 and both are active, the kernel
  // teleports the ray to the partner instead of shading it (portal_bonus.c).
  int                    portal[2];
} t_scene;
#endif

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
