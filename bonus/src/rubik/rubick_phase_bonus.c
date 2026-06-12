/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_phase_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:20 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:21 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/* A per-cubie tumble axis, varied but deterministic (and never zero). */
static cl_float3	tumble_axis(int c)
{
	return (make_float3(sinf(c * 1.7f + 0.3f),
			sinf(c * 2.6f + 1.1f), cosf(c * 1.3f + 0.5f)));
}

/* Rebuild every cubie spread out by `spread` and tumbled by `tumble`. */
static void	place_out(t_data *data, float spread, float tumble)
{
	t_rubik	*r;
	t_place	pl;
	int		c;

	r = &data->rubik;
	c = 0;
	while (c < CUBIES)
	{
		pl.cubie = c;
		pl.center = add(r->home_solved[c], scale(r->expl_dir[c], spread));
		pl.rot = quat_from_axis(tumble_axis(c), tumble);
		rubik_place_cubie(r, data->objects, pl);
		c++;
	}
}

/*
** Phase 1: fly the cubies apart (smoothstep so they ease to a stop) while
** each one tumbles. The tumble is a whole number of turns
** (EXPLODE_OUT_TURNS) so it lands back at the identity orientation, matching
** the spin/implode that follow.
*/
static void	phase_out(t_data *data)
{
	t_rubik	*r;
	float	spread;
	float	tumble;

	r = &data->rubik;
	r->explode_frame++;
	spread = EXPLODE_MAX_DIST
		* smoothstep((float)r->explode_frame / EXPLODE_OUT_FRAMES);
	data->scene_scale = 1.0f + (SCENE_SCALE_MAX - 1.0f)
		* smoothstep((float)r->explode_frame / EXPLODE_OUT_FRAMES);
	tumble = EXPLODE_OUT_TURNS * 2.0f * (float)CL_M_PI
		* ((float)r->explode_frame / EXPLODE_OUT_FRAMES);
	place_out(data, spread, tumble);
	r->spin_angle += EXPLODE_ORBIT_RATE;
	explode_camera(data, spread, r->spin_angle);
	if (r->explode_frame >= EXPLODE_OUT_FRAMES)
	{
		r->explode_phase = EXP_SPIN;
		r->explode_frame = 0;
	}
}

/*
** Phase 2: hold the spread cube and add a 720 deg burst on top of the steady
** orbit, eased slow-fast-slow (bullet time) via the smoothstep velocity
** curve.
*/
static void	phase_spin(t_data *data)
{
	t_rubik	*r;
	float	burst;

	r = &data->rubik;
	r->explode_frame++;
	burst = EXPLODE_SPIN_TURNS * 2.0f * (float)CL_M_PI
		* (smoothstep((float)r->explode_frame / EXPLODE_SPIN_FRAMES)
			- smoothstep((float)(r->explode_frame - 1) / EXPLODE_SPIN_FRAMES));
	r->spin_angle += EXPLODE_ORBIT_RATE + burst;
	explode_camera(data, EXPLODE_MAX_DIST, r->spin_angle);
	if (r->explode_frame >= EXPLODE_SPIN_FRAMES)
	{
		r->explode_phase = EXP_IN;
		r->explode_frame = 0;
	}
}

/* One frame of the cinematic; dispatches on the current phase. */
void	explode_step(t_data *data)
{
	if (data->rubik.explode_phase == EXP_OUT)
		phase_out(data);
	else if (data->rubik.explode_phase == EXP_SPIN)
		phase_spin(data);
	else if (data->rubik.explode_phase == EXP_IN)
		phase_in(data);
	else
		data->rubik.explode_active = 0;
}
