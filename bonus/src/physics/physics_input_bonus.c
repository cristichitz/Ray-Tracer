/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_input_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:41:40 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

void	apply_impulse(t_rbody *b, cl_float3 imp, cl_float3 point)
{
	if (b->inv_mass == 0.0f)
		return ;
	b->vel = add(b->vel, scale(imp, b->inv_mass));
	b->omega = add(b->omega,
			scale(cross(sub(point, b->pos), imp), b->inv_i));
	b->sleeping = 0;
}

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
