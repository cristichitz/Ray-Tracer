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

/*
** Add a ground quad and two emissive lights (one warm, one bluish) in the back
** corners, 45 deg up/behind the cube. Their indices and base offsets are saved
** so update_stage can scale them with the explosion.
*/
void	add_stage(t_data *data)
{
	float	h;

	data->scene_scale = 1.0f;
	data->stage_floor_y = data->rubik.center.y - RUBIK_STEP
		- RUBIK_STEP * 0.46f;
	data->light_base[0] = make_float3(STAGE_LIGHT_SPREAD, STAGE_LIGHT_HEIGHT,
			STAGE_LIGHT_BACK);
	data->light_base[1] = make_float3(-STAGE_LIGHT_SPREAD, STAGE_LIGHT_HEIGHT,
			STAGE_LIGHT_BACK);
	h = STAGE_FLOOR_HALF;
	data->stage_floor = (int)data->obj_count;
	add_object(data, make_obj_quad(make_float3(data->rubik.center.x - h,
				data->stage_floor_y, data->rubik.center.z - h),
			make_float3(2 * h, 0, 0), make_float3(0, 0, 2 * h),
			material_init(make_float3(0.45f, 0.45f, 0.48f), 0)));
	data->stage_light[0] = (int)data->obj_count;
	add_object(data, make_obj_sphere(add(data->rubik.center,
				data->light_base[0]), STAGE_LIGHT_RADIUS,
			material_init(make_float3(2.2f, 1.5f, 0.8f), 2)));
	data->stage_light[1] = (int)data->obj_count;
	add_object(data, make_obj_sphere(add(data->rubik.center,
				data->light_base[1]), STAGE_LIGHT_RADIUS,
			material_init(make_float3(0.5f, 0.9f, 2.6f), 2)));
	data->has_stage = 1;
}

/* Reposition + resize the floor and lights for the current scene_scale. */
void	update_stage(t_data *data)
{
	float	s;
	float	h;
	int		k;

	if (!data->has_stage)
		return ;
	s = data->scene_scale;
	k = 0;
	while (k < 2)
	{
		data->objects[data->stage_light[k]] = make_obj_sphere(
				add(data->rubik.center, scale(data->light_base[k], s)),
				STAGE_LIGHT_RADIUS * s,
				data->objects[data->stage_light[k]].material);
		k++;
	}
	h = STAGE_FLOOR_HALF * s;
	data->objects[data->stage_floor] = make_obj_quad(
			make_float3(data->rubik.center.x - h, data->stage_floor_y,
				data->rubik.center.z - h), make_float3(2 * h, 0, 0),
			make_float3(0, 0, 2 * h),
			data->objects[data->stage_floor].material);
}
