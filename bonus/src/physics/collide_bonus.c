/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:03 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:04 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

/*
** Impulse resolution for the falling cubies. Contacts are resolved with
** sequential impulses (normal bounce + Coulomb friction) plus Baumgarte
** positional correction so stacks don't sink. The cube's inertia is
** isotropic, so inv_i is a single scalar.
*/

/* Add velocity + angular velocity from an impulse applied at offset r. */
static void	body_apply(t_rbody *b, cl_float3 imp, cl_float3 r)
{
	b->vel = add(b->vel, scale(imp, b->inv_mass));
	b->omega = add(b->omega, scale(cross(r, imp), b->inv_i));
}

/* Clamped Coulomb friction along the tangent of the residual velocity. */
static void	friction_impulse(t_rbody *a, t_rbody *b, t_contact *c)
{
	cl_float3	relv;
	cl_float3	t;
	float		jt;

	relv = sub(add(b->vel, cross(b->omega, c->rb)),
			add(a->vel, cross(a->omega, c->ra)));
	t = sub(relv, scale(c->n, dot(relv, c->n)));
	if (dot(t, t) < 1e-10f)
		return ;
	t = norm(t);
	jt = -dot(relv, t) / (a->inv_mass + b->inv_mass
			+ a->inv_i * dot(cross(c->ra, t), cross(c->ra, t))
			+ b->inv_i * dot(cross(c->rb, t), cross(c->rb, t)));
	jt = fmaxf(-PHYS_FRICTION * c->jn, fminf(PHYS_FRICTION * c->jn, jt));
	body_apply(a, scale(t, -jt), c->ra);
	body_apply(b, scale(t, jt), c->rb);
}

/*
** Resolve one contact point cp with normal n (pointing from a to b): a
** normal impulse (restitution) followed by a clamped friction impulse.
*/
void	contact_impulse(t_rbody *a, t_rbody *b, cl_float3 n, cl_float3 cp)
{
	t_contact	c;
	cl_float3	relv;
	float		vn;
	float		denom;

	c.n = n;
	c.ra = sub(cp, a->pos);
	c.rb = sub(cp, b->pos);
	relv = sub(add(b->vel, cross(b->omega, c.rb)),
			add(a->vel, cross(a->omega, c.ra)));
	vn = dot(relv, n);
	if (vn >= 0.0f)
		return ;
	denom = a->inv_mass + b->inv_mass
		+ a->inv_i * dot(cross(c.ra, n), cross(c.ra, n))
		+ b->inv_i * dot(cross(c.rb, n), cross(c.rb, n));
	if (denom < 1e-8f)
		return ;
	c.jn = -(1.0f + PHYS_RESTITUTION) * vn / denom;
	body_apply(a, scale(n, -c.jn), c.ra);
	body_apply(b, scale(n, c.jn), c.rb);
	friction_impulse(a, b, &c);
}

/* The ground is an immovable plane; each sunk corner is a contact. */
void	collide_ground(t_rbody *b, float floor_y)
{
	t_rbody		ground;
	cl_float3	corner;
	float		maxpen;
	int			i;

	if (b->shape == 1)
	{
		collide_ball_ground(b, floor_y);
		return ;
	}
	memset(&ground, 0, sizeof(ground));
	maxpen = 0.0f;
	i = -1;
	while (++i < 8)
	{
		corner = box_vertex(b, i);
		if (corner.y < floor_y)
		{
			ground.pos = make_float3(corner.x, floor_y, corner.z);
			contact_impulse(&ground, b, make_float3(0.0f, 1.0f, 0.0f), corner);
			if (floor_y - corner.y > maxpen)
				maxpen = floor_y - corner.y;
		}
	}
	if (maxpen > PHYS_SLOP)
		b->pos.y += (maxpen - PHYS_SLOP) * PHYS_BAUMGARTE;
}

void	collide_pair(t_rbody *a, t_rbody *b)
{
	cl_float3	n;
	float		pen;
	float		corr;
	float		ti;

	n = make_float3(0.0f, 1.0f, 0.0f);
	pen = 0.0f;
	if (a->inv_mass == 0.0f && b->inv_mass == 0.0f)
		return ;
	if (a->shape == 1 || b->shape == 1)
	{
		collide_ball(a, b);
		return ;
	}
	if (!sat_overlap(a, b, &n, &pen))
		return ;
	contact_impulse(a, b, n, contact_point(a, b));
	ti = a->inv_mass + b->inv_mass;
	corr = fmaxf(0.0f, pen - PHYS_SLOP) * PHYS_BAUMGARTE;
	if (ti > 0.0f)
	{
		a->pos = sub(a->pos, scale(n, corr * a->inv_mass / ti));
		b->pos = add(b->pos, scale(n, corr * b->inv_mass / ti));
	}
}
