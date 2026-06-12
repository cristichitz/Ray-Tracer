#ifndef PLANE_H
# define PLANE_H

# include "hittable.h"
# include "quad.h"
# include "plane.h"
# include "stdlib.h"
typedef	struct s_box {
	t_hittable	base;
	t_quad		*sides[6];
} t_box;

t_box			*make_box(t_vec3 a, t_vec3 b, t_material mat);

#endif