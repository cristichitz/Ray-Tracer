/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wall_bricks_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 00:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 00:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/* World-space center of one brick; shared with the physics body setup. */
cl_float3	wall_brick_center(int row, int col)
{
	float	step;

	step = 2.0f * WALL_HALF + WALL_GAP;
	return (make_float3(
			((float)col - (WALL_COLS - 1) / 2.0f) * step,
			WALL_FLOOR_Y + WALL_HALF + WALL_GAP + (float)row * step,
			WALL_Z));
}

/* Checkerboard of terracotta and sand so the rubble reads nicely. */
static t_material	brick_mat(int row, int col)
{
	if ((row + col) % 2)
		return (material_init(make_float3(0.78f, 0.72f, 0.62f), 0));
	return (material_init(make_float3(0.66f, 0.27f, 0.21f), 0));
}

/*
** Stack WALL_ROWS x WALL_COLS bricks, 6 quads each, appended consecutively:
** brick i's first quad is at base + i * FACES. Bodies address their quads
** through that, so the wall size is not tied to the 27 rubik cubie slots.
*/
void	wall_bricks(t_data *data)
{
	cl_float3	c;
	cl_float3	h;
	int			row;
	int			col;

	h = make_float3(WALL_HALF, WALL_HALF, WALL_HALF);
	row = 0;
	while (row < WALL_ROWS)
	{
		col = 0;
		while (col < WALL_COLS)
		{
			c = wall_brick_center(row, col);
			make_box(data, sub(c, h), add(c, h), brick_mat(row, col));
			col++;
		}
		row++;
	}
}

/*
** Capture brick 0's six quads relative to its center as the canonical box
** (same trick as capture_cube in the explode cinematic), so a brick can be
** re-posed from just a body position + orientation.
*/
void	wall_capture_faces(t_data *data, int base)
{
	cl_float3	c0;
	int			f;

	c0 = wall_brick_center(0, 0);
	f = 0;
	while (f < FACES)
	{
		data->rubik.loc_c[f] = sub(data->objects[base + f].center, c0);
		data->rubik.loc_u[f] = data->objects[base + f].u;
		data->rubik.loc_v[f] = data->objects[base + f].v;
		f++;
	}
}

/* Re-pose one brick's 6 quads from its rigid body (b->obj = first quad). */
void	wall_place_brick(t_data *data, t_rbody *b)
{
	t_object	*o;
	cl_float3	n;
	int			f;

	f = 0;
	while (f < FACES)
	{
		o = &data->objects[b->obj + f];
		o->center = add(b->pos, quat_apply(b->orient, data->rubik.loc_c[f]));
		o->u = quat_apply(b->orient, data->rubik.loc_u[f]);
		o->v = quat_apply(b->orient, data->rubik.loc_v[f]);
		n = cross(o->u, o->v);
		o->normal = norm(n);
		o->d = dot(o->normal, o->center);
		o->w = divide(n, dot(n, n));
		f++;
	}
}
