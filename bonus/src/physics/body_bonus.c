/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   body_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:41:12 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>

static void	body_bounds(t_data *data, int first, int count, cl_float3 box[2])
{
	cl_float3	mn;
	cl_float3	mx;
	int			i;

	obj_bounds(&data->objects[first], &box[0], &box[1]);
	i = 1;
	while (i < count)
	{
		obj_bounds(&data->objects[first + i], &mn, &mx);
		box[0] = make_float3(fminf(box[0].x, mn.x), fminf(box[0].y, mn.y),
				fminf(box[0].z, mn.z));
		box[1] = make_float3(fmaxf(box[1].x, mx.x), fmaxf(box[1].y, mx.y),
				fmaxf(box[1].z, mx.z));
		i++;
	}
}

static void	capture_box(t_rbody *b, t_object *objs)
{
	int	f;

	f = 0;
	while (f < BOX_FACES)
	{
		b->loc_c[f] = sub(objs[b->obj_first + f].center, b->pos);
		b->loc_u[f] = objs[b->obj_first + f].u;
		b->loc_v[f] = objs[b->obj_first + f].v;
		f++;
	}
}

static void	body_shape(t_data *data, t_rbody *b)
{
	cl_float3	box[2];
	float		r;

	if (b->shape == 1)
	{
		b->pos = data->objects[b->obj_first].center;
		r = data->objects[b->obj_first].radius;
		b->half = make_float3(r, r, r);
		return ;
	}
	body_bounds(data, b->obj_first, b->obj_count, box);
	b->pos = scale(add(box[0], box[1]), 0.5f);
	b->half = scale(sub(box[1], box[0]), 0.5f);
	capture_box(b, data->objects);
}

static void	body_mass(t_rbody *b, float density)
{
	float	mass;
	float	vol;
	float	h2;

	if (b->shape == 1)
		vol = 4.18879f * b->half.x * b->half.x * b->half.x;
	else
		vol = 8.0f * b->half.x * b->half.y * b->half.z;
	mass = fmaxf(1e-6f, density * vol);
	b->inv_mass = 1.0f / mass;
	h2 = (b->half.x * b->half.x + b->half.y * b->half.y
			+ b->half.z * b->half.z) / 3.0f;
	if (b->shape == 1)
		b->inv_i = 2.5f * b->inv_mass / (b->half.x * b->half.x);
	else
		b->inv_i = 1.5f * b->inv_mass / h2;
}

void	add_body(t_data *data, int first, int count, t_material mat)
{
	t_rbody	*b;

	if (data->phys.count >= PHYS_MAX_BODIES)
		return ;
	b = &data->phys.bodies[data->phys.count];
	memset(b, 0, sizeof(*b));
	b->obj_first = first;
	b->obj_count = count;
	b->shape = (count == 1);
	b->orient = quat_identity();
	b->restitution = mat.restitution;
	b->friction = mat.friction;
	body_shape(data, b);
	body_mass(b, mat.density);
	data->phys.count++;
}
