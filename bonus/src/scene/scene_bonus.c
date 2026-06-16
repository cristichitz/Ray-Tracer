/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:12 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:13 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Append one primitive to the scene's flat array, growing it by one slot.
** Returns 1 on success, 0 on allocation failure.
*/
int	add_object(t_data *data, t_object obj)
{
	t_object	*grown;

	grown = (t_object *)realloc(data->objects,
			sizeof(t_object) * (data->obj_count + 1));
	if (!grown)
		return (parse_err("Failed to grow object array."));
	data->objects = grown;
	data->objects[data->obj_count] = obj;
	data->obj_count++;
	return (1);
}

/*
** Axis-aligned box as 6 quads. Ported from make_box() in
** mandatory/src/class/plane.c.
*/
int	make_box(t_data *data, cl_float3 a, cl_float3 b, t_material mat)
{
	cl_float3	mn;
	cl_float3	mx;
	cl_float3	dx;
	cl_float3	dy;
	cl_float3	dz;

	mn = make_float3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
	mx = make_float3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
	dx = make_float3(mx.x - mn.x, 0.0f, 0.0f);
	dy = make_float3(0.0f, mx.y - mn.y, 0.0f);
	dz = make_float3(0.0f, 0.0f, mx.z - mn.z);
	if (!add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mx.z),
				dx, dy, mat))
		|| !add_object(data, make_obj_quad(make_float3(mx.x, mn.y, mx.z),
				scale(dz, -1), dy, mat))
		|| !add_object(data, make_obj_quad(make_float3(mx.x, mn.y, mn.z),
				scale(dx, -1), dy, mat))
		|| !add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mn.z),
				dz, dy, mat))
		|| !add_object(data, make_obj_quad(make_float3(mn.x, mx.y, mx.z),
				dx, scale(dz, -1), mat))
		|| !add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mn.z),
				dx, dz, mat)))
		return (0);
	return (1);
}
