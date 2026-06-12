/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quat_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:38 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:39 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Small unit-quaternion toolkit shared by the explode cinematic and the
** rigid-body physics. A quaternion carries an orientation; rotating a vector
** by it is q * (0,v) * q^-1.
*/

t_quat	quat_identity(void)
{
	t_quat	q;

	q.w = 1.0f;
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = 0.0f;
	return (q);
}

/* Rotation by `angle` (radians) about a coordinate axis (0=x, 1=y, 2=z). */
t_quat	quat_axis(int axis, float angle)
{
	t_quat	q;
	float	s;

	s = sinf(angle * 0.5f);
	q.w = cosf(angle * 0.5f);
	q.x = (axis == 0) * s;
	q.y = (axis == 1) * s;
	q.z = (axis == 2) * s;
	return (q);
}

/* Rotation by `angle` (radians) about an arbitrary axis. */
t_quat	quat_from_axis(cl_float3 axis, float angle)
{
	t_quat	q;
	float	s;

	axis = norm(axis);
	s = sinf(angle * 0.5f);
	q.w = cosf(angle * 0.5f);
	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	return (q);
}

/* Hamilton product: the rotation "do b, then a". */
t_quat	quat_mul(t_quat a, t_quat b)
{
	t_quat	q;

	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	return (q);
}

/* Rotate a vector by a unit quaternion (q * v * q^-1). */
cl_float3	quat_apply(t_quat q, cl_float3 v)
{
	t_quat	p;
	t_quat	conj;
	t_quat	r;

	p.w = 0.0f;
	p.x = v.x;
	p.y = v.y;
	p.z = v.z;
	conj.w = q.w;
	conj.x = -q.x;
	conj.y = -q.y;
	conj.z = -q.z;
	r = quat_mul(quat_mul(q, p), conj);
	return (make_float3(r.x, r.y, r.z));
}
