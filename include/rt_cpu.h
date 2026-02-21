#ifndef RT_CPU_H
#define RT_CPU_H

#include "MLX42/MLX42.h"

//Sphere includes hittable...
#include "sphere.h"

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

typedef t_vec t_list;

typedef struct s_hittable_list {
   t_list *objects;
   int    (*add)(struct s_hittable_list *self, void *object);
   void   (*destroy)(struct s_hittable_list *self);
   bool   (*hit)(struct s_hittable_list *self, t_ray r, t_interval t, t_hit_record *rec);
} t_hittable_list;

typedef struct s_data {
    void      *mlx;
    void      *img;
    
    uint32_t  width;
    uint32_t  height;

    uint32_t  samples_per_pixel;
    float     pixel_samples_scale;
    
    // Camera
    float     cam_x;
    float     cam_y;
    float     cam_z;

    float     aspect_ratio;
    float     viewport_width;
    float     viewport_height;
    float     focal_length;

    t_vec3    origin;
    t_vec3    pixel00_loc;

    t_vec3    horizontal;
    t_vec3    vertical;

    t_vec3    pixel_width;
    t_vec3    pixel_height;

    // List of objects
    t_hittable_list world; 
} t_data;

// World
int       init_world(t_hittable_list  *world);

// MLX Loop
int       render_frame(t_data *data);
int       close_app(t_data *data);
int       key_hook(int keycode, t_data *data);

// Can be made inline if we add -O1 optimization for the compilation
// Random number generator
float random_float(float min, float max);
// Constants
float degrees_to_radians(float degrees);

#endif
