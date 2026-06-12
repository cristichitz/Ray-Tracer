/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_phase_2_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:23 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:24 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static cl_float3	lerp3(cl_float3 a, cl_float3 b, float t)
{
	return (add(scale(a, 1.0f - t), scale(b, t)));
}

/* End of phase 3: commit the scramble and let the cube solve itself. */
static void	finish_explode(t_rubik *r)
{
	int	c;

	c = 0;
	while (c < CUBIES)
	{
		r->cubies[c].pos[0] = r->target_pos[c][0];
		r->cubies[c].pos[1] = r->target_pos[c][1];
		r->cubies[c].pos[2] = r->target_pos[c][2];
		c++;
	}
	r->explode_phase = EXP_IDLE;
	r->explode_active = 0;
	r->orbit_active = 1;
	solve_rubik(r);
}

/*
** One cubie of phase 3: wait for its staggered launch, then accelerate
** (k = p*p) into place while its orientation slerps from solved to its
** scrambled target.
*/
static void	place_in(t_data *data, float gt, int c)
{
	t_rubik	*r;
	t_place	pl;
	float	p;

	r = &data->rubik;
	p = (gt - (float)c / CUBIES * EXPLODE_STAGGER)
		/ (1.0f - (float)c / CUBIES * EXPLODE_STAGGER);
	p = fmaxf(0.0f, fminf(1.0f, p));
	pl.cubie = c;
	pl.center = lerp3(add(r->home_solved[c],
				scale(r->expl_dir[c], EXPLODE_MAX_DIST)),
			r->home_target[c], p * p);
	pl.rot = quat_pow(r->rot_target[c], p * p);
	rubik_place_cubie(r, data->objects, pl);
}

/* Phase 3: suck the cubies into their scrambled homes one by one. */
void	phase_in(t_data *data)
{
	t_rubik	*r;
	float	gt;
	int		c;

	r = &data->rubik;
	r->explode_frame++;
	gt = (float)r->explode_frame / EXPLODE_IN_FRAMES;
	c = 0;
	while (c < CUBIES)
	{
		place_in(data, gt, c);
		c++;
	}
	r->spin_angle += EXPLODE_ORBIT_RATE;
	explode_camera(data, EXPLODE_MAX_DIST * (1.0f - smoothstep(gt)),
		r->spin_angle);
	if (r->explode_frame >= EXPLODE_IN_FRAMES)
		finish_explode(r);
}
