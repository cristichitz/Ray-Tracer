/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_3_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:41 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:42 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

cl_float3	random_vec_between(float min, float max)
{
	return (make_float3(random_float(min, max), random_float(min, max),
			random_float(min, max)));
}

cl_float3	random_unit_vector(void)
{
	cl_float3	p;
	float		lensq;

	while (1)
	{
		p = random_vec_between(-1.0f, 1.0f);
		lensq = length_squared(p);
		if (1e-32 < lensq && lensq <= 1)
			return (divide(p, sqrtf(lensq)));
	}
}

cl_float3	random_on_hemisphere(cl_float3 normal)
{
	cl_float3	on_unit_sphere;

	on_unit_sphere = random_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0f)
		return (on_unit_sphere);
	return (scale(on_unit_sphere, -1.0f));
}

bool	near_zero(cl_float3 a)
{
	float	s;

	s = 1e-8;
	return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}

void	print_vec(cl_float3 a)
{
	printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}
