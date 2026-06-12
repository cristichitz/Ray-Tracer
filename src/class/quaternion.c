/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quaternion.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:12:43 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 15:02:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quaternion.h"

t_quaternion	make_quaternion(t_vec3 axis, float angle)
{
	t_quaternion	q;
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

t_quaternion	mult_quaternion(t_quaternion a, t_quaternion b)
{
	t_quaternion	q;

	q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	return (q);
}

t_quaternion	inverse_quaternion(t_quaternion q)
{
	return ((t_quaternion){q.w, -q.x, -q.y, -q.z});
}

t_vec3	rotate_vec_by_quaternion(t_quaternion q, t_vec3 v)
{
	t_quaternion	base;
	t_quaternion	rotated_q;

	base = (t_quaternion){0.0f, v.x, v.y, v.z};
	rotated_q = mult_quaternion(mult_quaternion(q, base),
			inverse_quaternion(q));
	return (make_vec(rotated_q.x, rotated_q.y, rotated_q.z));
}
