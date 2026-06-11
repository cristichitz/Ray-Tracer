/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quaternion.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:12:43 by timurray          #+#    #+#             */
/*   Updated: 2026/06/08 18:57:42 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quarternion.h"
#include "vec3.h"

t_quarternion	make_quarternion(t_vec3 axis, float angle)
{
	t_quarternion	q;
	float			half;
	float			s;

	axis = norm(axis);
	half = angle * 0.5f;
	s = sinf(half);
	q.w = cosf(half);
	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	return (q);
}

t_quarternion	mult_quarternion(t_quarternion a, t_quarternion b)
{
	t_quarternion	q;

	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	return (q);
}

t_quarternion	inverse_quartenion(t_quarternion q)
{
	return ((t_quarternion){q.w, -q.x, -q.y, -q.z});
}

t_vec3	rotate_vec_by_quarternion(t_quarternion q, t_vec3 v)
{
	t_quarternion	base;
	t_quarternion	rotated_q;

	base = (t_quarternion){0.0f, v.x, v.y, v.z};
	rotated_q = mult_quarternion(mult_quarternion(q, base),
			inverse_quartenion(q));
	return (make_vec(rotated_q.x, rotated_q.y, rotated_q.z));
}
