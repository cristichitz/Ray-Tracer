#ifndef STRUCTS_H
#define STRUCTS_H

#ifdef  __OPENCL_VERSION__
    typedef float3  cl_float3;
#endif

typedef struct s_sphere
{
    cl_float3   center;
    float       radius;
    float       padding[3];
} t_sphere;

typedef struct s_hit_record
{
    cl_float3   p;
    cl_float3   normal;
    float       t;
    int         front_face;
}   t_hit_record;

typedef struct s_camera
{
    cl_float3   origin;
    cl_float3   pixel00_loc;
    cl_float3   pixel_delta_u;
    cl_float3   pixel_delta_v;
}   t_camera;

#endif