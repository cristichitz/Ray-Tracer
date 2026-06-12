/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quat_2_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:45 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:46 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** slerp(identity, q, t): the same rotation scaled to a fraction t of the way.
*/
t_quat	quat_pow(t_quat q, float t)
{
	t_quat	r;
	float	s;
	float	half;

	s = sqrtf(fmaxf(0.0f, 1.0f - q.w * q.w));
	if (s < 1e-6f)
		return (quat_identity());
	half = acosf(fmaxf(-1.0f, fminf(1.0f, q.w))) * t;
	r.w = cosf(half);
	r.x = q.x / s * sinf(half);
	r.y = q.y / s * sinf(half);
	r.z = q.z / s * sinf(half);
	return (r);
}

t_quat	quat_normalize(t_quat q)
{
	float	len;

	len = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z) + 1e-12f;
	q.w /= len;
	q.x /= len;
	q.y /= len;
	q.z /= len;
	return (q);
}

/* Advance an orientation by angular velocity w over dt (then renormalize). */
t_quat	quat_integrate(t_quat q, cl_float3 w, float dt)
{
	t_quat	d;
	t_quat	wq;

	wq.w = 0.0f;
	wq.x = w.x;
	wq.y = w.y;
	wq.z = w.z;
	d = quat_mul(wq, q);
	q.w += 0.5f * dt * d.w;
	q.x += 0.5f * dt * d.x;
	q.y += 0.5f * dt * d.y;
	q.z += 0.5f * dt * d.z;
	return (quat_normalize(q));
}

/*
** Rebuild one cubie's 6 quads from the canonical box at a center +
** orientation (packed in a t_place to stay within the parameter limit).
*/
void	rubik_place_cubie(t_rubik *r, t_object *objs, t_place pl)
{
	t_object	*o;
	cl_float3	n;
	int			f;

	f = 0;
	while (f < FACES)
	{
		o = &objs[r->cubies[pl.cubie].obj + f];
		o->center = add(pl.center, quat_apply(pl.rot, r->loc_c[f]));
		o->u = quat_apply(pl.rot, r->loc_u[f]);
		o->v = quat_apply(pl.rot, r->loc_v[f]);
		n = cross(o->u, o->v);
		o->normal = norm(n);
		o->d = dot(o->normal, o->center);
		o->w = divide(n, dot(n, n));
		f++;
	}
}
