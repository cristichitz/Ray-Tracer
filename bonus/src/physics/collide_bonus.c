/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:03 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:43:02 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static void	body_apply(t_rbody *b, cl_float3 imp, cl_float3 r)
{
	b->vel = add(b->vel, scale(imp, b->inv_mass));
	b->omega = add(b->omega, scale(cross(r, imp), b->inv_i));
}

static void	friction_impulse(t_rbody *a, t_rbody *b, t_contact *c)
{
	cl_float3	relv;
	cl_float3	t;
	float		jt;
	float		fr;

	relv = sub(add(b->vel, cross(b->omega, c->rb)),
			add(a->vel, cross(a->omega, c->ra)));
	t = sub(relv, scale(c->n, dot(relv, c->n)));
	if (dot(t, t) < 1e-10f)
		return ;
	t = norm(t);
	fr = sqrtf(a->friction * b->friction);
	jt = -dot(relv, t) / (a->inv_mass + b->inv_mass
			+ a->inv_i * dot(cross(c->ra, t), cross(c->ra, t))
			+ b->inv_i * dot(cross(c->rb, t), cross(c->rb, t)));
	jt = fmaxf(-fr * c->jn, fminf(fr * c->jn, jt));
	body_apply(a, scale(t, -jt), c->ra);
	body_apply(b, scale(t, jt), c->rb);
}

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
	c.jn = -(1.0f + 0.5f * (a->restitution + b->restitution)) * vn / denom;
	body_apply(a, scale(n, -c.jn), c.ra);
	body_apply(b, scale(n, c.jn), c.rb);
	friction_impulse(a, b, &c);
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
