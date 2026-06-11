/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 15:02:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 15:02:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ray.h"

t_vec3	ray_at(t_ray *self, float t)
{
	t_vec3	result;

	result = add(self->origin, scale(self->dir, t));
	return (result);
}

t_ray	make_ray(t_vec3 origin, t_vec3 direction)
{
	t_ray	r;

	r.origin = origin;
	r.dir = direction;
	r.at = ray_at;
	return (r);
}
