/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_input_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 16:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Modular way to poke the simulation. apply_impulse is the low-level primitive
** (a kick at a world point); ray_from_screen + pick_body turn a screen
** position into the body under it. Wiring a mouse click to "shove the sphere I
** clicked" is therefore just pick_body(ray_from_screen(...)) + apply_impulse,
** with no change to the rest of the engine.
*/

/* Add an impulse at a world point: changes linear and angular velocity. */
void	apply_impulse(t_rbody *b, cl_float3 imp, cl_float3 point)
{
	if (b->inv_mass == 0.0f)
		return ;
	b->vel = add(b->vel, scale(imp, b->inv_mass));
	b->omega = add(b->omega,
			scale(cross(sub(point, b->pos), imp), b->inv_i));
	b->sleeping = 0;
}

/* Nearest positive root of |origin + t*dir - c| = r (dir assumed unit). */
static float	ray_sphere(t_ray ray, cl_float3 c, float r)
{
	cl_float3	oc;
	float		h;
	float		disc;

	oc = sub(ray.origin, c);
	h = dot(oc, ray.dir);
	disc = h * h - (dot(oc, oc) - r * r);
	if (disc < 0.0f)
		return (-1.0f);
	return (-h - sqrtf(disc));
}

/* Index of the dynamic body the ray hits first, or -1 (uses bounding sphere). */
int	pick_body(t_data *data, t_ray ray)
{
	float	best;
	float	t;
	int		hit;
	int		i;

	best = 1e30f;
	hit = -1;
	i = 0;
	while (i < data->phys.count)
	{
		t = ray_sphere(ray, data->phys.bodies[i].pos,
				data->phys.bodies[i].half * 1.74f);
		if (t > 0.0f && t < best)
		{
			best = t;
			hit = i;
		}
		i++;
	}
	return (hit);
}

/* World-space ray through pixel (sx, sy), from the camera origin. */
t_ray	ray_from_screen(t_data *data, float sx, float sy)
{
	t_ray		ray;
	cl_float3	p;

	p = add(data->frame.pixel00_loc, scale(data->frame.pixel_delta_u, sx));
	p = add(p, scale(data->frame.pixel_delta_v, sy));
	ray.origin = data->cam_center;
	ray.dir = norm(sub(p, data->cam_center));
	return (ray);
}

/* SPACE: launch the body under the screen center along the view direction. */
void	shove_forward(t_data *data)
{
	t_ray	ray;
	int		i;

	ray = ray_from_screen(data, data->frame.width / 2.0f,
			data->frame.height / 2.0f);
	i = pick_body(data, ray);
	if (i < 0)
		return ;
	data->phys.bodies[i].vel = scale(data->cam_dir, PHYS_LAUNCH_SPEED);
	data->phys.bodies[i].sleeping = 0;
	data->phys.running = 1;
	data->phys.settle = 0;
}
