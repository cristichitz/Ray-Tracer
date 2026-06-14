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

/* One sub-iteration: integrate, resolve the static world, then every pair. */
static void	substep(t_physics *ph, float dt)
{
	int	i;
	int	j;

	i = 0;
	while (i < ph->count)
	{
		integrate(&ph->bodies[i], dt);
		if (ph->bodies[i].inv_mass > 0.0f)
			collide_world(ph, &ph->bodies[i]);
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

/* Advance the simulation one rendered frame and push poses into the geometry. */
void	physics_step(t_data *data)
{
	t_physics	*ph;
	float		dt;
	int			s;
	int			c;

	ph = &data->phys;
	if (!ph->running)
		return ;
	dt = PHYS_DT;
	if (data->render_mode)
		dt = RENDER_DT;
	s = 0;
	while (s++ < PHYS_SUBSTEPS)
		substep(ph, dt / PHYS_SUBSTEPS);
	c = 0;
	while (c < ph->count)
		body_place(data, &ph->bodies[c++]);
	if (peak_motion(ph) < PHYS_SLEEP_VEL)
		ph->settle++;
	else
		ph->settle = 0;
	if (ph->settle > 30)
		ph->running = 0;
}
