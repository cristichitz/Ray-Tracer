#ifndef SPHERE_H
# define SPHERE_H

# include "hittable.h"
# include "types.h"

typedef struct s_sphere
{
	t_hittable	base;
	t_vec3		center;
	float		radius;
	t_rgb		colour;
	t_material	mat;
}				t_sphere;

bool			hit_sphere(void *base, t_ray ray, t_interval t,
					t_hit_record *rec);
// t_sphere		*make_sphere(t_vec3 center, float radius);
t_sphere	*make_sphere(t_sphere sphere, t_material material);

#endif
