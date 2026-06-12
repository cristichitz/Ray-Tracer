/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_2_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:44 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:45 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

cl_float3	divide(cl_float3 a, float b)
{
	return (make_float3(a.x / b, a.y / b, a.z / b));
}

float	dot(cl_float3 a, cl_float3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

cl_float3	cross(cl_float3 a, cl_float3 b)
{
	return (make_float3(a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x));
}

cl_float3	norm(cl_float3 a)
{
	float	len;

	len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	len += 1e-8f;
	return (make_float3(a.x / len, a.y / len, a.z / len));
}

float	length_squared(cl_float3 a)
{
	return (a.x * a.x + a.y * a.y + a.z * a.z);
}
