/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:55:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:55:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static int	pixel_id(t_image *f)
{
	int	x;
	int	y;

	x = get_global_id(0);
	y = get_global_id(1);
	if (x >= f->width || y >= f->height)
		return (-1);
	return (y * f->width + x);
}

static float3	render_pixel(t_scene sc, t_image *f, int id, uint *seed)
{
	float3	color;
	t_ray	r;
	int		sample;

	color = (float3)(0.0f);
	sample = 0;
	while (sample < f->samples_per_pixel)
	{
		r = get_ray(f, id % f->width, id / f->width, seed);
		color += fmin(get_ray_color(sc, r, f, seed), (float3)(10.0f));
		sample++;
	}
	return (color * f->pixel_samples_scale);
}

static float3	accumulate(__global float4 *accum, int id, float3 color,
		int frame_index)
{
	float3	acc;

	if (frame_index == 0)
		acc = color;
	else
		acc = accum[id].xyz + color;
	accum[id] = (float4)(acc, 1.0f);
	return (acc);
}

__kernel void	render_kernel(__global int *img_buffer,
		__constant t_object *objects, int obj_count, t_image frame,
		__global float4 *accum, int frame_index,
		__global const t_bvh_node *nodes, __global const int *prim,
		int nnodes, int plane_first, int plane_count)
{
	t_scene	sc;
	int		id;
	uint	seed;
	float3	color;

	id = pixel_id(&frame);
	if (id < 0)
		return ;
	sc.objs = objects;
	sc.nodes = nodes;
	sc.prim = prim;
	sc.count = obj_count;
	sc.nnodes = nnodes;
	sc.plane_first = plane_first;
	sc.plane_count = plane_count;
	seed = (uint)id + (uint)frame_index * 9781u;
	pcg_hash(&seed);
	color = render_pixel(sc, &frame, id, &seed);
	color = accumulate(accum, id, color, frame_index);
	write_color(img_buffer, id, color / (float)(frame_index + 1));
}
