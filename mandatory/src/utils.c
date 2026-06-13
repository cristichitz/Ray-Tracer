/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:32:45 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 15:40:41 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

bool	is_emissive(t_hit_record *rec)
{
	t_vec3	emission;

	emission = rec->mat.emitted(&rec->mat, 0, 0, rec->p);
	return (emission.x > 0.0f || emission.y > 0.0f || emission.z > 0.0f);
}

t_vec3	sample_square(void)
{
	return (make_vec(random_float(0.0f, 1.0f) - 0.5, random_float(0.0f, 1.0f)
			- 0.5f, 0));
}

float	random_float(float min, float max)
{
	return (min + (max - min) * ((rand() / ((double)RAND_MAX + 1))));
}

float	deg_to_rad(float degrees)
{
	return (degrees * M_PI / 180.0f);
}

float	clampf(float val, float min, float max)
{
	if (val < min)
		return (min);
	if (val > max)
		return (max);
	return (val);
}
