/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:09 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:10 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static void	make_row(t_data *data, float y0, float y1, t_material mat)
{
	make_box(data, make_float3(-2.0f, y0, 15.0f),
		make_float3(2.0f, y1, 19.0f), mat);
	make_box(data, make_float3(-6.1f, y0, 15.0f),
		make_float3(-2.1f, y1, 19.0f), mat);
	make_box(data, make_float3(-10.2f, y0, 15.0f),
		make_float3(-6.2f, y1, 19.0f), mat);
}

void	make_rubick_cube(t_data *data)
{
	make_row(data, -2.0f, 2.0f,
		material_init(make_float3(0.0f, 0.0f, 1.0f), 0));
	make_row(data, 2.0f, 6.0f,
		material_init(make_float3(1.0f, 0.0f, 0.0f), 0));
	make_row(data, -2.0f, -6.0f,
		material_init(make_float3(0.0f, 0.5f, 0.0f), 0));
	data->cam_center = make_float3(0, 0, 0);
	data->cam_dir = make_float3(0, 0, 1);
	data->cam_fov = 70;
	data->frame.background = make_float3(0.564f, 0.835f, 1.0f);
}

static void	cornell_white(t_data *data)
{
	t_material	white;

	white = material_init(make_float3(0.73f, 0.73f, 0.73f), 0);
	add_object(data, make_obj_quad(make_float3(0, 0, 0),
			make_float3(555, 0, 0), make_float3(0, 0, 555), white));
	add_object(data, make_obj_quad(make_float3(555, 555, 555),
			make_float3(-555, 0, 0), make_float3(0, 0, -555), white));
	add_object(data, make_obj_quad(make_float3(0, 0, 555),
			make_float3(555, 0, 0), make_float3(0, 555, 0), white));
	make_box(data, make_float3(130, 0, 65), make_float3(295, 165, 230), white);
	make_box(data, make_float3(265, 0, 295),
		make_float3(430, 330, 460), white);
}

/*
** The classic Cornell box (5 walls + ceiling light + 2 boxes), ported from
** make_cornell_box() in mandatory/src/main_cpu.c. Also sets the camera and a
** black background so the only light is the emissive ceiling quad.
*/
void	make_cornell_box(t_data *data)
{
	t_material	red;
	t_material	green;
	t_material	light;

	red = material_init(make_float3(0.65f, 0.05f, 0.05f), 0);
	green = material_init(make_float3(0.12f, 0.45f, 0.15f), 0);
	light = material_init(make_float3(50.0f, 50.0f, 50.0f), 2);
	add_object(data, make_obj_quad(make_float3(555, 0, 0),
			make_float3(0, 555, 0), make_float3(0, 0, 555), green));
	add_object(data, make_obj_quad(make_float3(0, 0, 0),
			make_float3(0, 555, 0), make_float3(0, 0, 555), red));
	add_object(data, make_obj_quad(make_float3(343, 554, 332),
			make_float3(-130, 0, 0), make_float3(0, 0, -105), light));
	cornell_white(data);
	data->cam_center = make_float3(278, 278, -800);
	data->cam_dir = make_float3(0, 0, 1);
	data->cam_fov = 40;
	data->frame.background = make_float3(0, 0, 0);
}
