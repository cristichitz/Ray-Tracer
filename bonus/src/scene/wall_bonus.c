/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wall_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 00:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 00:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Cannonball demo room, Cornell-box style: white floor/ceiling/back wall,
** red and green side walls, no front wall (the camera looks in through the
** opening) and a single warm area light in the ceiling. The background is
** near-black so the quad light does all the work (sampled by NEE, so the
** soft shadows converge fast).
*/

/* Floor, ceiling and back wall in cornell white. */
static void	wall_room(t_data *d)
{
	t_material	white;
	cl_float3	wide;
	cl_float3	deep;

	white = material_init(make_float3(0.73f, 0.73f, 0.73f), 0);
	wide = make_float3(2.0f * WALL_ROOM_HX, 0.0f, 0.0f);
	deep = make_float3(0.0f, 0.0f, WALL_ROOM_BACK - WALL_ROOM_FRONT);
	add_object(d, make_obj_quad(make_float3(-WALL_ROOM_HX, WALL_FLOOR_Y,
				WALL_ROOM_FRONT), wide, deep, white));
	add_object(d, make_obj_quad(make_float3(-WALL_ROOM_HX, WALL_ROOM_TOP,
				WALL_ROOM_FRONT), wide, deep, white));
	add_object(d, make_obj_quad(make_float3(-WALL_ROOM_HX, WALL_FLOOR_Y,
				WALL_ROOM_BACK), wide,
			make_float3(0.0f, WALL_ROOM_TOP - WALL_FLOOR_Y, 0.0f), white));
}

/* Red / green side walls and the warm ceiling light over the brick wall. */
static void	wall_sides(t_data *d)
{
	cl_float3	deep;
	cl_float3	tall;

	deep = make_float3(0.0f, 0.0f, WALL_ROOM_BACK - WALL_ROOM_FRONT);
	tall = make_float3(0.0f, WALL_ROOM_TOP - WALL_FLOOR_Y, 0.0f);
	add_object(d, make_obj_quad(make_float3(-WALL_ROOM_HX, WALL_FLOOR_Y,
				WALL_ROOM_FRONT), deep, tall,
			material_init(make_float3(0.62f, 0.07f, 0.06f), 0)));
	add_object(d, make_obj_quad(make_float3(WALL_ROOM_HX, WALL_FLOOR_Y,
				WALL_ROOM_FRONT), deep, tall,
			material_init(make_float3(0.12f, 0.45f, 0.15f), 0)));
	add_object(d, make_obj_quad(make_float3(-5.0f, WALL_ROOM_TOP - 0.1f,
				12.0f), make_float3(10.0f, 0.0f, 0.0f),
			make_float3(0.0f, 0.0f, 12.0f),
			material_init(make_float3(14.0f, 12.5f, 10.0f), 2)));
}

/*
** Room, brick wall, metal cannonball (the last object added, which is what
** wall_physics_start expects), camera and a near-black background.
*/
void	build_wall_scene(t_data *data)
{
	int	base;

	wall_room(data);
	wall_sides(data);
	base = (int)data->obj_count;
	wall_bricks(data);
	wall_capture_faces(data, base);
	add_object(data, make_obj_sphere(make_float3(0.0f,
				WALL_FLOOR_Y + BALL_RADIUS, -2.0f), BALL_RADIUS,
			material_init(make_float3(0.85f, 0.85f, 0.88f), 1)));
	data->rubik.center = make_float3(0.0f, 0.0f, WALL_Z);
	data->cam_center = make_float3(0.0f, 2.0f, -24.0f);
	data->cam_dir = norm(make_float3(0.0f, -0.08f, 1.0f));
	data->cam_fov = 55.0f;
	data->frame.background = make_float3(0.02f, 0.02f, 0.03f);
	wall_physics_start(data, base);
}
