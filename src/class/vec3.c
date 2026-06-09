/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:57:12 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:57:20 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"

t_vec3	make_vec(float a, float b, float c)
{
	return ((t_vec3){a, b, c});
}

float	length_squared(t_vec3 a)
{
	return (a.x * a.x + a.y * a.y + a.z * a.z);
}

t_vec3	random_vec_between(float min, float max)
{
	return (make_vec(random_float(min, max), random_float(min, max),
			random_float(min, max)));
}

t_vec3	random_unit_vector(void)
{
	t_vec3	p;
	float	lensq;

	while (1)
	{
		p = random_vec_between(-1.0f, 1.0f);
		lensq = length_squared(p);
		if (1e-32 < lensq && lensq <= 1)
			return (divide(p, sqrtf(lensq)));
	}
}

t_vec3	random_on_hemisphere(t_vec3 normal)
{
	t_vec3	on_unit_sphere;

	on_unit_sphere = random_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0f)
		return (on_unit_sphere);
	else
		return (scale(on_unit_sphere, -1.0f));
}

bool	near_zero(t_vec3 a)
{
	float	s;

	s = 1e-8;
	return (fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s);
}

t_vec3	mult(t_vec3 a, t_vec3 b)
{
	return (make_vec(a.x * b.x, a.y * b.y, a.z * b.z));
}

t_vec3	add(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x + b.x, a.y + b.y, a.z + b.z});
}

t_vec3	sub(t_vec3 a, t_vec3 b)
{
	return ((t_vec3){a.x - b.x, a.y - b.y, a.z - b.z});
}

t_vec3	scale(t_vec3 a, float b)
{
	return ((t_vec3){a.x * b, a.y * b, a.z * b});
}

t_vec3	divide(t_vec3 a, float b)
{
	return ((t_vec3){a.x / b, a.y / b, a.z / b});
}

float	dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vec3	cross(t_vec3 a, t_vec3 b)
{
	return (make_vec(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y
			- a.y * b.x));
}

t_vec3	norm(t_vec3 a)
{
	float	len;

	len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	return (make_vec(a.x / len, a.y / len, a.z / len));
}

void	print_vec(t_vec3 a)
{
	printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}
