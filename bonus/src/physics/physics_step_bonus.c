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

/*
** A carried body is driven toward the cursor target by a capped velocity (not
** teleported), so it still collides with walls and cannot be shoved through
** them. Gravity is off while held; the spin is bled away so it hangs steady.
*/
static void	drive_held(t_rbody *b, float dt, cl_float3 target)
{
	cl_float3	d;
	float		dist;
	float		speed;

	d = sub(target, b->pos);
	dist = sqrtf(dot(d, d));
	speed = fminf(dist / dt, HOLD_MAX_SPEED);
	if (dist > 1e-4f)
		b->vel = scale(d, speed / dist);
	else
		b->vel = make_float3(0.0f, 0.0f, 0.0f);
	b->pos = add(b->pos, scale(b->vel, dt));
	b->omega = scale(b->omega, 0.5f);
}

/* Semi-implicit Euler step for one dynamic body. */
static void	integrate(t_rbody *b, float dt, cl_float3 target)
{
	if (b->inv_mass == 0.0f || b->sleeping)
		return ;
	if (b->held)
		return (drive_held(b, dt, target));
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
		integrate(&ph->bodies[i], dt, ph->hold_target);
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

/*
** Aim the carry constraint: store the world point along the ray through the
** current mouse position that the held body is driven toward (drive_held).
** Recomputed each frame so the cube tracks the cursor and can be dropped
** anywhere.
*/
static void	hold_aim(t_data *data)
{
	cl_float3	t;
	t_ray		ray;
	float		floor;
	int			x;
	int			y;

	if (data->phys.held < 0)
		return ;
	mlx_get_mouse_pos(data->mlx, &x, &y);
	ray = ray_from_screen(data, (float)x, (float)y);
	t = add(ray.origin, scale(ray.dir, data->phys.hold_dist));
	floor = data->phys.floor_y + data->phys.bodies[data->phys.held].half;
	if (t.y < floor)
		t.y = floor;
	data->phys.hold_target = t;
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
	hold_aim(data);
	portal_sync_holes(data);
	s = 0;
	while (s++ < PHYS_SUBSTEPS)
		substep(ph, dt / PHYS_SUBSTEPS);
	portals_teleport(data);
	c = 0;
	while (c < ph->count)
		body_place(data, &ph->bodies[c++]);
	if (peak_motion(ph) < PHYS_SLEEP_VEL && ph->held < 0)
		ph->settle++;
	else
		ph->settle = 0;
	if (ph->settle > 30)
		ph->running = 0;
}
