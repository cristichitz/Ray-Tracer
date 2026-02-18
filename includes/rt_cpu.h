#ifndef RT_CPU_H
#define RT_CPU_H

#include <stdlib.h>
#include <sys/time.h>
#include "MLX42/MLX42.h"
#include <stdbool.h>
#include "libft.h"

//objects
#include "vec3.h"
typedef t_vec t_list;

typedef struct s_ray {
  t_vec3 origin;
  t_vec3 dir;
  t_vec3 (*at)(struct s_ray *self, float t);
} t_ray;

typedef struct s_hit_record {
  t_vec3  p;
  t_vec3  normal;
  float   t;
  bool    front_face;
  
  void    (*set_face_normal)(struct s_hit_record *self, t_ray ray, t_vec3 outward_normal);
} t_hit_record;

typedef struct s_hittable_list {
   t_list *objects;
   void   (*add)(struct s_hittable_list *self, void *object);
   void   (*destroy)(struct s_hittable_list *self);
   bool   (*hit)(struct s_hittable_list *self, t_ray r, float t_min, t_hit_record *rec);
} t_hittable_list;

// Sphere object
typedef struct s_sphere {
  t_vec3  center;
  float   radius;
  bool   (*hit)(struct s_sphere *self, t_ray ray, float t_min, float t_max, t_hit_record *rec);
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


bool      hit_sphere(t_sphere *self, t_ray ray, float t_min, float t_max, t_hit_record *rec);
void      init_sphere(t_data *data);
t_sphere* make_sphere(t_vec3 center, float radius);


t_vec3  make_vec(float a, float b, float c);
int     render_frame(t_data *data);
int     close_app(t_data *data);
int     key_hook(int keycode, t_data *data);

// World
int  init_world(t_hittable_list  *world);

// Ray
t_ray make_ray(t_vec3 origin, t_vec3 direction);
#endif
