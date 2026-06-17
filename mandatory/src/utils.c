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

t_vec3	safe_right_vec(t_vec3 forward)
{
	t_vec3	right;

	right = cross(make_vec(0.0f, 1.0f, 0.0f), forward);
	if (length_squared(right) < 1e-8f)
		right = make_vec(1.0f, 0.0f, 0.0f);
	return (norm(right));
}
