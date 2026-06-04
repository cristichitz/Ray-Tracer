#ifndef VEC3_H
# define VEC3_H

# include <math.h>
# include <stdio.h>
# include <types.h>
# include <stdbool.h>

// Can be made inline if we add -O1 optimization for the compilation
// Random number generator
float random_float(float min, float max);
// Constants
float degrees_to_radians(float degrees);

float   length_squared(t_vec3 a);
t_vec3  random_vec_between(float min, float max);
t_vec3  random_unit_vector(void);
t_vec3  random_on_hemisphere(t_vec3 normal);

bool near_zero(t_vec3 a);
t_vec3 mult(t_vec3 a, t_vec3 b);

t_vec3	make_vec(float a, float b, float c);
t_vec3	add(t_vec3 a, t_vec3 b);
t_vec3	sub(t_vec3 a, t_vec3 b);
t_vec3	scale(t_vec3 a, float b);
t_vec3	divide(t_vec3 a, float b);
float	dot(t_vec3 a, t_vec3 b);
t_vec3	norm(t_vec3 a);
void	print_vec(t_vec3 a);

#endif
