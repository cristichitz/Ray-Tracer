/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_rocket_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:51 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:52 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** The finger's grand exit, like a helicopter: after a short hold it spins up
** in place about its vertical axis (accelerating), and only once the rotor
** is wound up (PHYS_LIFT_DELAY) does it accelerate upward out of frame, then
** freeze. The cubies move as one rigid group (the spin is applied to both
** their offset from the axis and their orientation).
*/

static void	rocket_pose(t_physics *ph, float *spin, float *rise)
{
	float	lift;

	*spin = (float)(ph->rocket_frame - PHYS_ROCKET_DELAY);
	*spin = 0.5f * PHYS_SPIN_ACCEL * *spin * *spin;
	lift = fmaxf(0.0f, (float)(ph->rocket_frame - PHYS_LIFT_DELAY));
	*rise = 0.5f * PHYS_RISE_ACCEL * lift * lift;
	if (*rise > PHYS_ROCKET_MAX_H)
	{
		*rise = PHYS_ROCKET_MAX_H;
		ph->finger_done = 1;
	}
}

static void	rocket_place(t_data *data, t_quat rot, float rise, int c)
{
	t_rubik		*r;
	cl_float3	off;

	r = &data->rubik;
	off = quat_apply(rot, make_float3(r->cubies[c].pos[0] * RUBIK_STEP,
				0.0f, r->cubies[c].pos[2] * RUBIK_STEP));
	data->phys.bodies[c].pos = make_float3(r->center.x + off.x, r->center.y
			+ r->cubies[c].pos[1] * RUBIK_STEP + rise + off.y,
			r->center.z + off.z);
	data->phys.bodies[c].orient = rot;
}

void	rocket_finger(t_data *data)
{
	t_physics	*ph;
	float		rise;
	float		spin;
	int			c;

	ph = &data->phys;
	if (ph->finger_done || ++ph->rocket_frame <= PHYS_ROCKET_DELAY)
		return ;
	rocket_pose(ph, &spin, &rise);
	c = -1;
	while (++c < ph->count)
	{
		if (cubie_is_finger(&data->rubik, c))
			rocket_place(data, quat_axis(1, spin), rise, c);
	}
}
