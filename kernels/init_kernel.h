#ifndef INIT_KERNEL_H
#define INIT_KERNEL_H


#ifdef  __OPENCL_VERSION__
  typedef float3  cl_float3;
#endif

#include "internals.h"

#define RAND_MAX 2147483647

typedef struct s_sphere {
  cl_float3   center;
  float       radius;
  t_material  material;
} t_sphere;

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
} t_image;

/* float3  refract(float3 uv, float3 n, float etai_over_etat) */
/* { */
/*   float   cos_theta; */
/*   float3  r_out_perp; */
/*   float3  r_out_parallel; */
/*   float   sqrt_ray; */
/**/
/*   cos_theta = fmin(dot(-uv, n), 1.0f); */
/*   r_out_perp = etai_over_etat * (uv + cos_theta*n); */
/*   sqrt_ray = -sqrt(fabs(1.0 - dot(r_out_perp, r_out_perp))); */
/*   r_out_parallel = (float3)(n.x * sqrt_ray, n.y * sqrt_ray, n.z * sqrt_ray); */
/*   return (r_out_perp + r_out_parallel); */
/* } */
/**/
/* bool scatter_dielectric(t_material *self, t_ray r_in, t_hit_record rec, float3 *attenuation, t_ray *scattered) */
/* { */
/*   float   ri; */
/*   float3  unit_direction; */
/*   float3  refracted; */
/**/
/*   *attenuation = (float3)(1.0, 1.0, 1.0); */
/*   if (rec.front_face) */
/*     ri = 1.0 / self->albedo.x; */
/*   else */
/*     ri = self->albedo.x; */
/**/
/*   unit_direction = normalize(r_in.dir); */
/*   refracted = refract(unit_direction, rec.normal, ri); */
/*   *scattered = make_ray(rec.p, refracted); */
/*   return (true); */
/* } */
#endif
