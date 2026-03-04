#ifndef VEC3_H
# define VEC3_H

# include <math.h>
# include <stdio.h>
# include <types.h>

t_vec3	make_vec(float a, float b, float c);
t_vec3	add(t_vec3 a, t_vec3 b);
t_vec3	sub(t_vec3 a, t_vec3 b);
t_vec3	scale(t_vec3 a, float b);
t_vec3	divide(t_vec3 a, float b);
float	dot(t_vec3 a, t_vec3 b);
t_vec3	norm(t_vec3 a);
void	print_vec(t_vec3 a);

#endif
