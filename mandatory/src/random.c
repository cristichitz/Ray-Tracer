/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 17:48:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/14 17:48:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"
#include <sys/time.h>

static unsigned int	g_rng_state;

void	init_random(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	g_rng_state = (unsigned int)(tv.tv_usec ^ tv.tv_sec);
	if (g_rng_state == 0)
		g_rng_state = 42;
}

float	random_float(float min, float max)
{
	g_rng_state ^= g_rng_state << 13;
	g_rng_state ^= g_rng_state >> 17;
	g_rng_state ^= g_rng_state << 5;
	return (min + (max - min) * (g_rng_state / 4294967296.0f));
}

t_vec3	sample_square(void)
{
	return (make_vec(random_float(0.0f, 1.0f) - 0.5, random_float(0.0f, 1.0f)
			- 0.5f, 0));
}
