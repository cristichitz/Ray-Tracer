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
** Static collision planes harvested from the scene. A collider is the
** half-space n . p >= d with n pointing toward the bodies, so a floor, a back
** wall or a ramp are all just .rt planes. A temporary immovable body carries
** the collider's restitution/friction into the shared contact solver.
*/

/* Sphere vs one collision plane. */
static void	plane_ball(t_rbody *b, t_collider c)
{
	t_rbody	wall;
	float	pen;

	pen = c.d - (dot(b->pos, c.n) - b->half);
	if (pen <= 0.0f)
		return ;
	memset(&wall, 0, sizeof(wall));
	wall.restitution = c.restitution;
	wall.friction = c.friction;
	wall.pos = sub(b->pos, scale(c.n, b->half));
	contact_impulse(&wall, b, c.n, wall.pos);
	if (pen > PHYS_SLOP)
		b->pos = add(b->pos, scale(c.n, (pen - PHYS_SLOP) * PHYS_BAUMGARTE));
}

/* Box vs one collision plane: every corner past the plane is a contact. */
static void	plane_box(t_rbody *b, t_collider c)
{
	t_rbody		wall;
	cl_float3	corner;
	float		maxpen;
	int			i;

	memset(&wall, 0, sizeof(wall));
	wall.restitution = c.restitution;
	wall.friction = c.friction;
	maxpen = 0.0f;
	i = -1;
	while (++i < 8)
	{
		corner = box_vertex(b, i);
		if (dot(corner, c.n) < c.d)
		{
			wall.pos = corner;
			contact_impulse(&wall, b, c.n, corner);
			maxpen = fmaxf(maxpen, c.d - dot(corner, c.n));
		}
	}
	if (maxpen > PHYS_SLOP)
		b->pos = add(b->pos, scale(c.n, (maxpen - PHYS_SLOP) * PHYS_BAUMGARTE));
}

/* Resolve a body against one collision plane (shape-dispatched). */
void	collide_plane(t_rbody *b, t_collider c)
{
	if (b->shape == 1)
		plane_ball(b, c);
	else
		plane_box(b, c);
}

/*
** Collide one body against the whole static world. Scenes define their floor
** and walls as planes; with none, a single flat ground at floor_y is the
** fallback so a lone falling object still lands on something.
*/
void	collide_world(t_physics *ph, t_rbody *b)
{
	int	i;

	if (ph->collider_count == 0)
	{
		collide_ground(b, ph->floor_y);
		return ;
	}
	i = 0;
	while (i < ph->collider_count)
	{
		collide_plane(b, ph->colliders[i]);
		i++;
	}
}
