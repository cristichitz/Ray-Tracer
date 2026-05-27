#ifndef RT_BONUS_H
#define RT_BONUS_H

#include "CL/cl.h" 
#include "MLX42/MLX42.h"
//Sphere inclues hittable...
// #include "shape.h"
#include "cl_util_bonus.h"
// #include "sphere.h"
// #include "hittable.h"
#include "init_kernel.h"
// #include "vec3.h"
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

float random_float(float min, float max);
float degrees_to_radians(float degrees);

cl_float3 make_float3(cl_float x, cl_float y, cl_float z);

float   length_squared(cl_float3 a);
cl_float3  random_vec_between(float min, float max);
cl_float3  random_unit_vector(void);
cl_float3  random_on_hemisphere(cl_float3 normal);

bool near_zero(cl_float3 a);
cl_float3 mult(cl_float3 a, cl_float3 b);

cl_float3  make_vec(float a, float b, float c);
cl_float3  add(cl_float3 a, cl_float3 b);
cl_float3  sub(cl_float3 a, cl_float3 b);
cl_float3  scale(cl_float3 a, float b);
cl_float3  divide(cl_float3 a, float b);
float      dot(cl_float3 a, cl_float3 b);
cl_float3  norm(cl_float3 a);
void       print_vec(cl_float3 a);

typedef struct s_gpu {
  cl_context        context;
  cl_command_queue  queue;
  cl_program        program;
  cl_kernel         kernel;
  cl_mem            buffer;
  cl_mem            sphere_buffer;
} t_gpu;

typedef struct s_data {
    void          *mlx;
    mlx_image_t   *img;

    float cam_x;
    float cam_y;
    float cam_z;

    t_gpu     gpu;
    cl_int    err;
    
    t_image   frame;
 
    // List of objects
    t_sphere  *spheres;
    uint32_t  sphere_count;
} t_data;

// World
// int       init_world(t_hittable_list  *world);

// MLX Loop
// int       render_frame(t_data *data);
int       close_app(t_data *data);
int       key_hook(int keycode, t_data *data);

//SPHERE
t_sphere make_sphere(cl_float3 center, float radius, t_material mat);
#endif
