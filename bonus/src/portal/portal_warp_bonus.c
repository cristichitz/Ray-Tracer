/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portal_warp_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/15 12:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Map a world point through the portal pair (the host twin of portal_warp in
** the kernel): the coordinates of p in src's frame are reproduced in dst's
** frame with the 180-degree about-up flip, so a body that sank c units behind
** src re-emerges c units in front of dst.
*/
cl_float3	portal_point(t_object *src, t_object *dst, cl_float3 p)
{
	cl_float3	rel;
	cl_float3	out;
	float		x;
	float		y;
	float		c;

	rel = sub(p, src->center);
	x = dot(rel, norm(src->u));
	y = dot(rel, norm(src->v));
	c = dot(rel, src->normal);
	out = add(dst->center, scale(norm(dst->u), -x));
	out = add(out, scale(norm(dst->v), y));
	out = add(out, scale(dst->normal, -c));
	return (out);
}

/* Same mapping for a free vector (velocity / angular velocity): no translation. */
cl_float3	portal_vec(t_object *src, t_object *dst, cl_float3 raw)
{
	cl_float3	out;
	float		a;
	float		b;
	float		c;

	a = dot(raw, norm(src->u));
	b = dot(raw, norm(src->v));
	c = dot(raw, src->normal);
	out = scale(norm(dst->u), -a);
	out = add(out, scale(norm(dst->v), b));
	out = add(out, scale(dst->normal, -c));
	return (out);
}

/*
** True once the body's leading face has reached portal o (within `half` of its
** plane) and its center sits inside the ellipse. Triggering at the face, not
** the center, is what stops a cube from sinking halfway in before it pops out.
*/
static int	crossed_portal(t_object *o, cl_float3 pos, float half)
{
	cl_float3	rel;
	float		pu;
	float		pv;

	rel = sub(pos, o->center);
	if (dot(rel, o->normal) > half)
		return (0);
	pu = dot(rel, o->u) / dot(o->u, o->u);
	pv = dot(rel, o->v) / dot(o->v, o->v);
	return (pu * pu + pv * pv <= 1.0f);
}

/*
** If body b has reached portal s, transport its pose to the partner. The
** in-plane offset maps through portal_point; the perpendicular distance is
** then forced to half + a hair so the body emerges fully in front of dst
** (never buried behind the destination wall).
*/
static void	try_teleport(t_data *data, t_rbody *b, int s)
{
	t_object	*src;
	t_object	*dst;
	float		perp;

	if (!data->portal_active[s] || !data->portal_active[1 - s])
		return ;
	src = &data->objects[data->portal_obj[s]];
	dst = &data->objects[data->portal_obj[1 - s]];
	if (!crossed_portal(src, b->pos, b->half))
		return ;
	b->pos = portal_point(src, dst, b->pos);
	perp = dot(sub(b->pos, dst->center), dst->normal);
	b->pos = add(b->pos, scale(dst->normal, (b->half + 0.3f) - perp));
	b->vel = portal_vec(src, dst, b->vel);
	b->omega = portal_vec(src, dst, b->omega);
	b->portal_cd = PORTAL_COOLDOWN;
	b->sleeping = 0;
}

/*
** Mirror the active portals' geometry into the physics state so the collision
** solver can open a "hole" in any collider an ellipse sits on (the cube falls
** through the ellipse instead of landing on the surface behind it).
*/
void	portal_sync_holes(t_data *data)
{
	t_object	*o;
	int			n;
	int			i;

	n = 0;
	data->phys.portal_hole = 0;
	if (!data->portal_active[0] || !data->portal_active[1])
		return ;
	i = 0;
	while (i < 2)
	{
		if (data->portal_active[i])
		{
			o = &data->objects[data->portal_obj[i]];
			data->phys.hole_c[n] = o->center;
			data->phys.hole_n[n] = o->normal;
			data->phys.hole_u[n] = o->u;
			data->phys.hole_v[n] = o->v;
			n++;
		}
		i++;
	}
	data->phys.portal_hole = n;
}

/* Per-frame: tick each body's cooldown, then test it against both portals. */
void	portals_teleport(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->phys.count)
	{
		if (data->phys.bodies[i].portal_cd > 0)
			data->phys.bodies[i].portal_cd--;
		else if (!data->phys.bodies[i].held)
		{
			try_teleport(data, &data->phys.bodies[i], 0);
			try_teleport(data, &data->phys.bodies[i], 1);
		}
		i++;
	}
}
