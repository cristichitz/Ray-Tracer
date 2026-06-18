/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_ball_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 00:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 00:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

/* Rotate v by the inverse of unit quaternion q (the conjugate). */
static cl_float3	quat_unapply(t_quat q, cl_float3 v)
{
	t_quat	c;

	c.w = q.w;
	c.x = -q.x;
	c.y = -q.y;
	c.z = -q.z;
	return (quat_apply(c, v));
}

/* Closest point of box b (as an OBB volume) to the world point p. */
static cl_float3	closest_on_box(t_rbody *box, cl_float3 p)
{
	cl_float3	l;

	l = quat_unapply(box->orient, sub(p, box->pos));
	l.x = fmaxf(-box->half.x, fminf(box->half.x, l.x));
	l.y = fmaxf(-box->half.y, fminf(box->half.y, l.y));
	l.z = fmaxf(-box->half.z, fminf(box->half.z, l.z));
	return (add(box->pos, quat_apply(box->orient, l)));
}

/*
** Sphere vs OBB: one contact at the closest point, normal box -> ball.
** A center fully inside the box (dist ~ 0) is skipped; the substep rate is
** high enough that the ball can never tunnel that deep in one step.
*/
static void	ball_box(t_rbody *ball, t_rbody *box)
{
	cl_float3	cp;
	cl_float3	n;
	float		dist;
	float		corr;
	float		ti;

	cp = closest_on_box(box, ball->pos);
	n = sub(ball->pos, cp);
	dist = sqrtf(dot(n, n));
	if (dist >= ball->half.x || dist < 1e-6f)
		return ;
	n = divide(n, dist);
	contact_impulse(box, ball, n, cp);
	ti = ball->inv_mass + box->inv_mass;
	corr = fmaxf(0.0f, (ball->half.x - dist) - PHYS_SLOP) * PHYS_BAUMGARTE;
	if (ti > 0.0f)
	{
		ball->pos = add(ball->pos, scale(n, corr * ball->inv_mass / ti));
		box->pos = sub(box->pos, scale(n, corr * box->inv_mass / ti));
	}
}

/* Dispatch so collide_pair stays shape-agnostic for any sphere/box pair. */
void	collide_ball(t_rbody *a, t_rbody *b)
{
	if (a->shape == 1 && b->shape == 1)
		ball_ball(a, b);
	else if (a->shape == 1)
		ball_box(a, b);
	else
		ball_box(b, a);
}

/* Sphere vs the immovable ground plane. */
void	collide_ball_ground(t_rbody *b, float floor_y)
{
	t_rbody		ground;
	cl_float3	cp;
	float		pen;

	pen = floor_y - (b->pos.y - b->half.x);
	if (pen <= 0.0f)
		return ;
	memset(&ground, 0, sizeof(ground));
	ground.restitution = b->restitution;
	ground.friction = b->friction;
	cp = make_float3(b->pos.x, floor_y, b->pos.z);
	ground.pos = cp;
	contact_impulse(&ground, b, make_float3(0.0f, 1.0f, 0.0f), cp);
	if (pen > PHYS_SLOP)
		b->pos.y += (pen - PHYS_SLOP) * PHYS_BAUMGARTE;
}
