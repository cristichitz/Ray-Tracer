/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_wall_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 00:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 00:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

/* One standing brick: unit mass, at rest, waiting to be knocked over. */
static void	wall_brick_body(t_data *data, int c, int brick_base)
{
	t_rbody	*b;

	b = &data->phys.bodies[c];
	memset(b, 0, sizeof(*b));
	b->obj = brick_base + c * FACES;
	b->half = WALL_HALF;
	b->orient = quat_identity();
	b->pos = wall_brick_center(c / WALL_COLS, c % WALL_COLS);
	b->inv_mass = 1.0f;
	b->inv_i = 1.5f / (WALL_HALF * WALL_HALF);
}

/* The cannonball: a heavy sphere body driving the scene's last object. */
static void	wall_ball_body(t_data *data, int obj)
{
	t_rbody	*b;

	b = &data->phys.bodies[data->phys.count - 1];
	memset(b, 0, sizeof(*b));
	b->shape = 1;
	b->obj = obj;
	b->half = BALL_RADIUS;
	b->orient = quat_identity();
	b->pos = data->objects[obj].center;
	b->inv_mass = BALL_INV_MASS;
	b->inv_i = 2.5f * BALL_INV_MASS / (BALL_RADIUS * BALL_RADIUS);
}

/*
** Set up every body but leave the simulation paused (running = 0), so the
** wall stands perfectly still until the ball is fired. finger_done = 1 lets
** the regular settle logic stop the sim once the rubble stops moving.
*/
void	wall_physics_start(t_data *data, int brick_base)
{
	int	c;

	c = 0;
	while (c < WALL_COLS * WALL_ROWS)
	{
		wall_brick_body(data, c, brick_base);
		c++;
	}
	data->phys.count = WALL_COLS * WALL_ROWS + 1;
	wall_ball_body(data, (int)data->obj_count - 1);
	data->phys.wall_mode = 1;
	data->phys.finger_done = 1;
	data->phys.floor_y = WALL_FLOOR_Y;
	data->phys.running = 0;
}

/* SPACE: hurl the cannonball at the wall (re-fires from wherever it lies). */
void	wall_fire(t_data *data)
{
	t_rbody	*b;

	b = &data->phys.bodies[data->phys.count - 1];
	b->vel = make_float3(0.0f, BALL_LIFT, BALL_SPEED);
	b->omega = make_float3(8.0f, 0.0f, 0.0f);
	b->sleeping = 0;
	data->phys.settle = 0;
	data->phys.running = 1;
}
