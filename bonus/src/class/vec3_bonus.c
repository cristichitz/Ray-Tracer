/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:38 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:39 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

cl_float3	make_float3(float x, float y, float z)
{
	cl_float3	v;

	v.s[0] = x;
	v.s[1] = y;
	v.s[2] = z;
	v.s[3] = 0.0f;
	return (v);
}

cl_float3	add(cl_float3 a, cl_float3 b)
{
	return (make_float3(a.x + b.x, a.y + b.y, a.z + b.z));
}

cl_float3	sub(cl_float3 a, cl_float3 b)
{
	return (make_float3(a.x - b.x, a.y - b.y, a.z - b.z));
}

cl_float3	mult(cl_float3 a, cl_float3 b)
{
	return (make_float3(a.x * b.x, a.y * b.y, a.z * b.z));
}

cl_float3	scale(cl_float3 a, float b)
{
	return (make_float3(a.x * b, a.y * b, a.z * b));
}
