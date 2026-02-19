#ifndef RT_CPU_H
#define RT_CPU_H

#include "MLX42/MLX42.h"
//Sphere inclues hittable...
#include "sphere.h"

// Constants
inline float degrees_to_radians(float degrees)
{
  return (degrees * M_PI / 180.0f);
}

typedef t_vec t_list;

typedef struct s_hittable_list {
   t_list *objects;
   int    (*add)(struct s_hittable_list *self, void *object);
   void   (*destroy)(struct s_hittable_list *self);
   bool   (*hit)(struct s_hittable_list *self, t_ray r, t_interval t, t_hit_record *rec);
} t_hittable_list;

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
    t_hittable_list world;
    
} t_data;

// World
int       init_world(t_hittable_list  *world);

// MLX Loop
int       render_frame(t_data *data);
int       close_app(t_data *data);
int       key_hook(int keycode, t_data *data);


#endif
