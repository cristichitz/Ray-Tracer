/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:54 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:41:28 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static void	add_collider(t_physics *ph, t_object *o)
{
	t_collider	*c;

	if (ph->collider_count >= PHYS_MAX_COLLIDERS)
		return ;
	c = &ph->colliders[ph->collider_count];
	c->n = o->normal;
	c->d = o->d;
	c->restitution = o->material.restitution;
	c->friction = o->material.friction;
	ph->collider_count++;
}

void	physics_init(t_data *data)
{
	t_physics	*ph;
	uint32_t	i;

	ph = &data->phys;
	ph->collider_count = 0;
	i = 0;
	while (i < data->obj_count)
	{
		if (data->objects[i].type == OBJ_PLANE
			&& !data->objects[i].material.dynamic)
			add_collider(ph, &data->objects[i]);
		i++;
	}
	ph->floor_y = 0.0f;
	ph->settle = 0;
	ph->autostart = 0;
	ph->running = 0;
}
