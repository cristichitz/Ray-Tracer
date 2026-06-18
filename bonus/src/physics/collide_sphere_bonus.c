/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_sphere_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 16:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Sphere vs sphere: one contact at the touch point on the line of centers,
** normal a -> b. Needed now that any .rt scene can hold many dynamic spheres
** (the old single-cannonball demo never had two balls at once). Penetration is
** split between the two bodies by inverse mass, like collide_pair does.
*/
void	ball_ball(t_rbody *a, t_rbody *b)
{
	cl_float3	n;
	float		dist;
	float		corr;
	float		ti;

	n = sub(b->pos, a->pos);
	dist = sqrtf(dot(n, n));
	if (dist >= a->half.x + b->half.x || dist < 1e-6f)
		return ;
	n = divide(n, dist);
	contact_impulse(a, b, n, add(a->pos, scale(n, a->half.x)));
	ti = a->inv_mass + b->inv_mass;
	corr = fmaxf(0.0f, (a->half.x + b->half.x - dist) - PHYS_SLOP)
		* PHYS_BAUMGARTE;
	if (ti > 0.0f)
	{
		a->pos = sub(a->pos, scale(n, corr * a->inv_mass / ti));
		b->pos = add(b->pos, scale(n, corr * b->inv_mass / ti));
	}
}
