/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:32:45 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 18:03:56 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

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