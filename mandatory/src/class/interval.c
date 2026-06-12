/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:52:06 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:55:10 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interval.h"

bool	contains(t_interval *self, float x)
{
	return (self->min <= x && x <= self->max);
}

bool	surrounds(t_interval *self, float x)
{
	return (self->min < x && x < self->max);
}

float	clamp(t_interval *self, float x)
{
	if (x < self->min)
		return (self->min);
	if (x > self->max)
		return (self->max);
	return (x);
}

t_interval	expand(t_interval *self, float delta)
{
	float	padding;

	padding = delta / 2;
	return (interval_init(self->min - padding, self->max + padding));
}

t_interval	interval_init(float t_min, float t_max)
{
	t_interval	i;

	i.min = t_min;
	i.max = t_max;
	i.size = t_max - t_min;
	i.contains = contains;
	i.surrounds = surrounds;
	i.clamp = clamp;
	i.expand = expand;
	return (i);
}
