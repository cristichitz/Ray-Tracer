/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_plan_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:18 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <stdlib.h>

/*
** Scramble planning for the explode cinematic: choose SCRAMBLE_LEN random
** moves and, without animating anything, work out where every cubie ends up.
*/

/* Rotate an integer grid coordinate by 90*turns about axis (matches snap). */
static void	rotate_grid(int *p, int axis, int turns)
{
	cl_float3	v;
	float		st;
	int			k;

	st = 1.0f - 2.0f * (turns < 0);
	v = make_float3(p[0], p[1], p[2]);
	k = abs(turns);
	while (k-- > 0)
		rot_vec_axis(&v, axis, 0.0f, st);
	p[0] = (int)lroundf(v.x);
	p[1] = (int)lroundf(v.y);
	p[2] = (int)lroundf(v.z);
}

/* Start tracking from the current grid state with identity orientations. */
static void	init_tracks(t_rubik *r, int (*tp)[3], t_quat *tq)
{
	int	c;

	c = 0;
	while (c < CUBIES)
	{
		tp[c][0] = r->cubies[c].pos[0];
		tp[c][1] = r->cubies[c].pos[1];
		tp[c][2] = r->cubies[c].pos[2];
		tq[c] = quat_identity();
		c++;
	}
}

/* Apply one move to every tracked cubie in its slice. */
static void	apply_move(int (*tp)[3], t_quat *tq, t_move m)
{
	int	c;

	c = 0;
	while (c < CUBIES)
	{
		if (tp[c][m.axis] == m.layer)
		{
			rotate_grid(tp[c], m.axis, m.turns);
			tq[c] = quat_mul(quat_axis(m.axis,
						degrees_to_radians(90.0f * m.turns)), tq[c]);
		}
		c++;
	}
}

/* Commit the tracked end state: grid cell, world center, net orientation. */
static void	store_targets(t_rubik *r, int (*tp)[3], t_quat *tq)
{
	int	c;

	c = 0;
	while (c < CUBIES)
	{
		r->target_pos[c][0] = tp[c][0];
		r->target_pos[c][1] = tp[c][1];
		r->target_pos[c][2] = tp[c][2];
		r->rot_target[c] = tq[c];
		r->home_target[c] = add(r->center, scale(make_float3(tp[c][0],
						tp[c][1], tp[c][2]), RUBIK_STEP));
		c++;
	}
}

/*
** The moves are stored in history so solve_rubik can later replay their
** inverse.
*/
void	plan_scramble(t_rubik *r)
{
	int		trackpos[CUBIES][3];
	t_quat	trackq[CUBIES];
	t_move	m;
	int		i;

	init_tracks(r, trackpos, trackq);
	r->h_count = 0;
	i = 0;
	while (i < SCRAMBLE_LEN)
	{
		m.axis = rand() % 3;
		m.layer = (rand() % 2) * 2 - 1;
		m.turns = (rand() % 3) - 1;
		if (m.turns == 0)
			m.turns = 2;
		r->history[r->h_count++] = m;
		apply_move(trackpos, trackq, m);
		i++;
	}
	store_targets(r, trackpos, trackq);
}
