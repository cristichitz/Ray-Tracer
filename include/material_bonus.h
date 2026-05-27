#ifndef MATERIAL_BONUS_H
#define MATERIAL_BONUS_H

#include "hittable_bonus.h"

typedef struct s_material {
  t_vec3  albedo; 
  bool    (*scatter)(struct s_material *self, t_ray r_in, t_hit_record *rec, t_vec3 *attenuation, t_ray *scattered);
} t_material;


t_material init_lambertian(t_vec3 col);
t_material init_metal(t_vec3 col);

#endif  
