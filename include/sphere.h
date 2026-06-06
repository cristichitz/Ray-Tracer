#ifndef SPHERE_H
# define SPHERE_H

# include "hittable.h"
# include "types.h"

typedef struct s_sphere
{
	t_hittable	base;
	t_vec3		center;
	float		radius;
	t_material	mat;
}				t_sphere;

t_sphere		*make_sphere(t_sphere sphere);

#endif
