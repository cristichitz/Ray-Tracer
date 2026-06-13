/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_step_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:49 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:49 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/* Semi-implicit Euler step for one dynamic body. */
static void	integrate(t_rbody *b, float dt)
{
	if (b->inv_mass == 0.0f || b->sleeping)
		return ;
	b->vel.y += PHYS_GRAVITY * dt;
	b->vel = scale(b->vel, PHYS_LIN_DAMP);
	b->omega = scale(b->omega, PHYS_ANG_DAMP);
	b->pos = add(b->pos, scale(b->vel, dt));
	b->orient = quat_integrate(b->orient, b->omega, dt);
}

/* One collision sub-iteration: integrate, then resolve ground + every pair. */
static void	substep(t_physics *ph, float dt)
{
	int	i;
	int	j;

	i = 0;
	while (i < ph->count)
	{
		integrate(&ph->bodies[i], dt);
		if (ph->bodies[i].inv_mass > 0.0f)
		{
			collide_ground(&ph->bodies[i], ph->floor_y);
			if (ph->wall_mode)
				collide_room(&ph->bodies[i]);
		}
		i++;
	}
	i = 0;
	while (i < ph->count)
	{
		j = i;
		while (++j < ph->count)
			collide_pair(&ph->bodies[i], &ph->bodies[j]);
		i++;
	}
}

/* Largest linear+angular speed among the dynamic bodies (sleep detection). */
static float	peak_motion(t_physics *ph)
{
	float	m;
	float	s;
	int		i;

	m = 0.0f;
	i = 0;
	while (i < ph->count)
	{
		if (ph->bodies[i].inv_mass > 0.0f)
		{
			s = dot(ph->bodies[i].vel, ph->bodies[i].vel)
				+ dot(ph->bodies[i].omega, ph->bodies[i].omega);
			if (s > m)
				m = s;
		}
		i++;
	}
	return (sqrtf(m));
}

/* Push every body's pose into the quad geometry the GPU renders. */
static void	push_bodies(t_data *data)
{
	t_physics	*ph;
	t_place		pl;
	int			c;

	ph = &data->phys;
	c = 0;
	while (c < ph->count)
	{
		if (ph->bodies[c].shape == 1)
			data->objects[ph->bodies[c].obj].center = ph->bodies[c].pos;
		else if (ph->wall_mode)
			wall_place_brick(data, &ph->bodies[c]);
		else
		{
			pl.cubie = ph->bodies[c].cubie;
			pl.center = ph->bodies[c].pos;
			pl.rot = ph->bodies[c].orient;
			rubik_place_cubie(&data->rubik, data->objects, pl);
		}
		c++;
	}
}

/* Advance the simulation one rendered frame and push the result to geometry. */
void	physics_step(t_data *data)
{
	t_physics	*ph;
	float		dt;
	int			s;

	ph = &data->phys;
	if (!ph->running)
		return ;
	if (!ph->wall_mode)
		rocket_finger(data);
	dt = PHYS_DT;
	if (data->render_mode)
		dt = RENDER_DT;
	s = 0;
	while (s++ < PHYS_SUBSTEPS)
		substep(ph, dt / PHYS_SUBSTEPS);
	push_bodies(data);
	if (peak_motion(ph) < PHYS_SLEEP_VEL)
		ph->settle++;
	else
		ph->settle = 0;
	if (ph->finger_done && ph->settle > 30)
		ph->running = 0;
}
