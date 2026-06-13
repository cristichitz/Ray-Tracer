/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_room_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 01:10:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 01:10:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

/*
** Rigid room walls for the --wall demo. Each wall is the half-space
** n . p >= d with n the unit normal pointing INTO the room, matching the
** visible quads (WALL_ROOM_*), so nothing tumbles through the scenery.
*/

/* Sphere vs one wall plane. */
static void	room_plane_ball(t_rbody *b, cl_float3 n, float d)
{
	t_rbody	wall;
	float	pen;

	pen = d - (dot(b->pos, n) - b->half);
	if (pen <= 0.0f)
		return ;
	memset(&wall, 0, sizeof(wall));
	wall.pos = sub(b->pos, scale(n, b->half));
	contact_impulse(&wall, b, n, wall.pos);
	if (pen > PHYS_SLOP)
		b->pos = add(b->pos, scale(n, (pen - PHYS_SLOP) * PHYS_BAUMGARTE));
}

/* Box vs one wall plane: every sunk corner becomes a contact. */
static void	room_plane_box(t_rbody *b, cl_float3 n, float d)
{
	t_rbody		wall;
	cl_float3	corner;
	float		maxpen;
	int			i;

	memset(&wall, 0, sizeof(wall));
	maxpen = 0.0f;
	i = -1;
	while (++i < 8)
	{
		corner = box_vertex(b, i);
		if (dot(corner, n) < d)
		{
			wall.pos = corner;
			contact_impulse(&wall, b, n, corner);
			if (d - dot(corner, n) > maxpen)
				maxpen = d - dot(corner, n);
		}
	}
	if (maxpen > PHYS_SLOP)
		b->pos = add(b->pos, scale(n, (maxpen - PHYS_SLOP) * PHYS_BAUMGARTE));
}

static void	room_plane(t_rbody *b, cl_float3 n, float d)
{
	if (b->shape == 1)
		room_plane_ball(b, n, d);
	else
		room_plane_box(b, n, d);
}

/* The two side walls, the back wall and the invisible front opening. */
void	collide_room(t_rbody *b)
{
	room_plane(b, make_float3(1.0f, 0.0f, 0.0f), -WALL_ROOM_HX);
	room_plane(b, make_float3(-1.0f, 0.0f, 0.0f), -WALL_ROOM_HX);
	room_plane(b, make_float3(0.0f, 0.0f, -1.0f), -WALL_ROOM_BACK);
	room_plane(b, make_float3(0.0f, 0.0f, 1.0f), WALL_ROOM_FRONT);
}
