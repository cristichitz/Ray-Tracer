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
