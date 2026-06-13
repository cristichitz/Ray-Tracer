/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 17:41:06 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 17:49:45 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"

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
	return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}
