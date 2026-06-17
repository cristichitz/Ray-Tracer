/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:03:06 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:03:45 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internals.h"

t_interval	interval_init(float t_min, float t_max)
{
	t_interval	i;

	i.min = t_min;
	i.max = t_max;
	i.size = t_max - t_min;
	return (i);
}

bool	interval_contains(t_interval *self, float x)
{
	return (self->min <= x && x <= self->max);
}

bool	interval_surrounds(t_interval *self, float x)
{
	return (self->min < x && x < self->max);
}

float	interval_clamp(t_interval *self, float x)
{
	if (x < self->min)
		return (self->min);
	if (x > self->max)
		return (self->max);
	return (x);
}
