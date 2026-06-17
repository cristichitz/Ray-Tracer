/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:00:39 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:00:39 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internals.h"

t_ray	make_ray(float3 origin, float3 direction)
{
	t_ray	r;

	r.origin = origin;
	r.dir = direction;
	return (r);
}

float3	ray_at(t_ray *self, float t)
{
	return (self->origin + (self->dir * t));
}

void	ft_set_face_normal(t_hit_record *self, t_ray r, float3 outward_normal)
{
	self->front_face = dot(r.dir, outward_normal) < 0;
	if (self->front_face != true)
		outward_normal = outward_normal * -1.0f;
	self->normal = outward_normal;
}
