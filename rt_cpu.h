#ifndef RT_CPU_H
#define RT_CPU_H

#include <stdlib.h>
#include <sys/time.h>
#include "MLX42/MLX42.h"
#include <stdbool.h>

//objects
#include "vec3.h"

typedef struct s_ray {
  t_vec3 origin;
  t_vec3 dir;
  t_vec3 (*at)(struct s_ray *self, float t);
} t_ray;

typedef struct s_hit_record {
  t_vec3  p;
  t_vec3  normal;
  float   t;
} t_hit_record;

// Sphere object
typedef struct s_sphere {
  t_vec3  center;
  float   radius;
  float   (*hit)(struct s_sphere *self, t_ray ray);
} t_sphere;

typedef struct s_data {
    void    *mlx;
    void    *img;
    
    uint32_t     width;
    uint32_t    height;
    
    // Camera
    float   cam_x;
    float   cam_y;
    float   cam_z;

    float aspect_ratio;
    float viewport_width;
    float viewport_height;
    float focal_length;

    t_sphere sphere;
} t_data;


float hit_sphere(t_sphere *self, t_ray ray);
void  init_sphere(t_data *data);


t_vec3  make_vec(float a, float b, float c);
int     render_frame(t_data *data);
int     close_app(t_data *data);
int     key_hook(int keycode, t_data *data);

#endif
