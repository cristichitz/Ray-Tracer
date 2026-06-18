/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_box_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:59 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:59 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** OBB geometry helpers for box rigid bodies: world-space box axes, corner
** positions, point containment and the contact-point approximation.
*/

static float	half_sign(float half, int neg)
{
	if (neg)
		return (half);
	return (-half);
}

/* The i-th body axis (its local x/y/z rotated into the world). */
cl_float3	box_axis(t_rbody *b, int i)
{
	if (i == 0)
		return (quat_apply(b->orient, make_float3(1.0f, 0.0f, 0.0f)));
	if (i == 1)
		return (quat_apply(b->orient, make_float3(0.0f, 1.0f, 0.0f)));
	return (quat_apply(b->orient, make_float3(0.0f, 0.0f, 1.0f)));
}

/* World position of corner i (bits select the +/- half on each local axis). */
cl_float3	box_vertex(t_rbody *b, int i)
{
	cl_float3	local;

	local = make_float3(half_sign(b->half.x, i & 1),
			half_sign(b->half.y, i & 2), half_sign(b->half.z, i & 4));
	return (add(b->pos, quat_apply(b->orient, local)));
}

static int	point_in_box(t_rbody *b, cl_float3 p)
{
	t_quat		conj;
	cl_float3	local;

	conj.w = b->orient.w;
	conj.x = -b->orient.x;
	conj.y = -b->orient.y;
	conj.z = -b->orient.z;
	local = quat_apply(conj, sub(p, b->pos));
	return (fabsf(local.x) <= b->half.x + 1e-4f
		&& fabsf(local.y) <= b->half.y + 1e-4f
		&& fabsf(local.z) <= b->half.z + 1e-4f);
}

/* Approximate the contact as the centroid of mutually-contained corners. */
cl_float3	contact_point(t_rbody *a, t_rbody *b)
{
	cl_float3	acc;
	cl_float3	v;
	int			cnt;
	int			i;

	acc = make_float3(0.0f, 0.0f, 0.0f);
	cnt = 0;
	i = -1;
	while (++i < 16)
	{
		if (i < 8)
			v = box_vertex(b, i);
		else
			v = box_vertex(a, i - 8);
		if (point_in_box(a, v) && point_in_box(b, v))
		{
			acc = add(acc, v);
			cnt++;
		}
	}
	if (cnt == 0)
		return (scale(add(a->pos, b->pos), 0.5f));
	return (scale(acc, 1.0f / cnt));
}
