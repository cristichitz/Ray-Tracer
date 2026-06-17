/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_ground_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:45:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:45:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

/* Resolve every box corner below the floor; returns deepest penetration. */
static float	ground_contacts(t_rbody *ground, t_rbody *b, float floor_y)
{
	cl_float3	corner;
	float		maxpen;
	int			i;

	maxpen = 0.0f;
	i = -1;
	while (++i < 8)
	{
		corner = box_vertex(b, i);
		if (corner.y < floor_y)
		{
			ground->pos = make_float3(corner.x, floor_y, corner.z);
			contact_impulse(ground, b, make_float3(0.0f, 1.0f, 0.0f), corner);
			if (floor_y - corner.y > maxpen)
				maxpen = floor_y - corner.y;
		}
	}
	return (maxpen);
}

/* The ground is an immovable plane; each sunk corner becomes a contact. */
void	collide_ground(t_rbody *b, float floor_y)
{
	t_rbody	ground;
	float	maxpen;

	if (b->shape == 1)
	{
		collide_ball_ground(b, floor_y);
		return ;
	}
	memset(&ground, 0, sizeof(ground));
	ground.restitution = b->restitution;
	ground.friction = b->friction;
	maxpen = ground_contacts(&ground, b, floor_y);
	if (maxpen > PHYS_SLOP)
		b->pos.y += (maxpen - PHYS_SLOP) * PHYS_BAUMGARTE;
}
