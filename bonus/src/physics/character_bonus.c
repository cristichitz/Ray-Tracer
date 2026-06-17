/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   character_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 15:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/16 15:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** The player is a rigid body like any other (so it falls, collides and travels
** through portals for free); it is just driven by the keyboard instead of by
** gravity alone. Created once at load with its rotation locked (inv_i = 0) so
** it stays upright, and parked on the floor until M switches to character mode.
*/
void	spawn_character(t_data *data)
{
	t_material	mat;
	cl_float3	c;
	cl_float3	h;
	int			base;

	mat = material_init(make_float3(0.95f, 0.55f, 0.15f), 0);
	mat.dynamic = 1;
	mat.restitution = 0.0f;
	mat.friction = 0.7f;
	c = make_float3(0.0f, data->phys.floor_y + CHAR_SIZE * 0.5f + 0.05f, -18.0f);
	h = make_float3(CHAR_SIZE * 0.5f, CHAR_SIZE * 0.5f, CHAR_SIZE * 0.5f);
	base = (int)data->obj_count;
	if (!make_box(data, sub(c, h), add(c, h), mat))
		return ;
	add_body(data, base, BOX_FACES, mat);
	data->phys.character = data->phys.count - 1;
	data->phys.bodies[data->phys.character].inv_i = 0.0f;
}

/* Desired horizontal velocity from WASD, relative to where the camera looks. */
static cl_float3	walk_dir(t_data *data)
{
	cl_float3	fwd;
	cl_float3	right;
	cl_float3	d;

	fwd = norm(make_float3(data->cam_dir.x, 0.0f, data->cam_dir.z));
	right = norm(cross(make_float3(0.0f, 1.0f, 0.0f), fwd));
	d = make_float3(0.0f, 0.0f, 0.0f);
	if (mlx_is_key_down(data->mlx, MLX_KEY_W))
		d = add(d, fwd);
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		d = sub(d, fwd);
	if (mlx_is_key_down(data->mlx, MLX_KEY_D))
		d = add(d, right);
	if (mlx_is_key_down(data->mlx, MLX_KEY_A))
		d = sub(d, right);
	if (dot(d, d) > 1e-4f)
		return (scale(norm(d), CHAR_SPEED));
	return (make_float3(0.0f, 0.0f, 0.0f));
}

/* On the floor (and not rising), so a jump is allowed. */
static int	grounded(t_rbody *b, float floor)
{
	return (b->pos.y - b->half <= floor + 0.3f && b->vel.y <= 1.0f);
}

/*
** Drive the avatar each frame: WASD set the horizontal velocity outright (so it
** stops dead when you release the keys), gravity keeps the vertical component,
** and SPACE launches a jump when grounded. Rotation is kept zeroed so the body
** never tips over.
*/
void	character_input(t_data *data)
{
	t_rbody		*b;
	cl_float3	d;

	if (data->phys.character < 0)
		return ;
	b = &data->phys.bodies[data->phys.character];
	d = walk_dir(data);
	b->vel.x = d.x;
	b->vel.z = d.z;
	if (mlx_is_key_down(data->mlx, MLX_KEY_SPACE)
		&& grounded(b, data->phys.floor_y))
		b->vel.y = CHAR_JUMP;
	b->omega = make_float3(0.0f, 0.0f, 0.0f);
	b->sleeping = 0;
	data->phys.running = 1;
	data->phys.settle = 0;
}

/* Pin the camera at eye height above the avatar (gives the player a height). */
void	character_camera(t_data *data)
{
	t_rbody	*b;

	if (data->phys.character < 0)
		return ;
	b = &data->phys.bodies[data->phys.character];
	data->cam_center = add(b->pos,
			make_float3(0.0f, b->half + EYE_HEIGHT, 0.0f));
	update_view(data);
}
