/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 17:40:02 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 17:40:03 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"

t_vec3	divide(t_vec3 a, float b)
{
	return ((t_vec3){a.x / b, a.y / b, a.z / b});
}

float	dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vec3	cross(t_vec3 a, t_vec3 b)
{
	return (make_vec(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y
			- a.y * b.x));
}

t_vec3	norm(t_vec3 a)
{
	float	len;

	len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	return (make_vec(a.x / len, a.y / len, a.z / len));
}
