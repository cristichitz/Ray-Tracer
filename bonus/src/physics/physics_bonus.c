/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:54 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:55 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** The simulation is built entirely from the parsed scene: dynamic-material
** objects became rigid bodies as they were parsed (add_body), and here every
** static plane becomes an immovable collision half-space. The .rt file alone
** therefore defines both what moves and what it collides against.
*/

/* Harvest one static plane as a collider; its normal points toward the bodies. */
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

/*
** Scan the scene for static planes -> colliders, then leave the simulation
** PAUSED. A paused scene renders its bodies at their parsed poses and the path
** tracer converges to a clean image; SPACE (fire / start) or a left click
** (shove) wakes it. The step loop re-pauses once everything settles again, so
** the picture is only ever noisy while something is actually moving.
*/
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
	ph->held = -1;
	ph->hold_dist = 0.0f;
}
