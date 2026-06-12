/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_engine_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:32 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:33 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Pop the next queued move and set up its animation: MOVE_FRAMES frames, each
** advancing the slice by an equal slice of the total angle.
*/
static void	start_move(t_rubik *r)
{
	r->current = r->queue[r->q_head];
	r->q_head = (r->q_head + 1) % MAX_MOVES;
	r->q_count--;
	r->frames_left = MOVE_FRAMES;
	r->step = degrees_to_radians(90.0f * r->current.turns) / MOVE_FRAMES;
	r->active = 1;
}

/*
** When idle, start the next queued move; with an empty queue, hand over to
** the physics finale if the orbit cinematic was running. Returns 0 when
** there is nothing to animate this frame.
*/
static int	next_move(t_data *data, t_rubik *r)
{
	if (r->q_count == 0)
	{
		if (r->orbit_active)
		{
			r->orbit_active = 0;
			physics_start(data);
		}
		return (0);
	}
	start_move(r);
	return (1);
}

/*
** One frame of the move engine. Call it every frame before render_frame:
** advances the current turn, or starts the next queued one when idle.
*/
void	step_rubik(t_data *data)
{
	t_rubik	*r;

	r = &data->rubik;
	if (r->explode_active)
	{
		explode_step(data);
		return ;
	}
	if (!r->active && !next_move(data, r))
		return ;
	rotate_layer(r, data->objects, r->step);
	r->frames_left--;
	if (r->frames_left == 0)
	{
		snap_layer(r);
		r->active = 0;
	}
	if (r->orbit_active)
		explode_orbit_solve(data);
}
