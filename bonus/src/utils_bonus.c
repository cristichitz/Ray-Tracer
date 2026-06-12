/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:54 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:51:55 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

float	random_float(float min, float max)
{
	return (min + (max - min) * (rand() / ((double)RAND_MAX + 1)));
}

float	degrees_to_radians(float degrees)
{
	return (degrees * M_PI / 180.0f);
}
