/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_bounds_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 23:55:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 23:55:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static cl_float3	vmin3(cl_float3 a, cl_float3 b)
{
	return (make_float3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)));
}

static cl_float3	vmax3(cl_float3 a, cl_float3 b)
{
	return (make_float3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)));
}

/* Tight box over the 4 corners, padded so a flat quad is never zero-thin. */
static void	quad_bounds(t_object *o, cl_float3 *mn, cl_float3 *mx)
{
	cl_float3	c;

	*mn = o->center;
	*mx = o->center;
	c = add(o->center, o->u);
	*mn = vmin3(*mn, c);
	*mx = vmax3(*mx, c);
	c = add(o->center, o->v);
	*mn = vmin3(*mn, c);
	*mx = vmax3(*mx, c);
	c = add(add(o->center, o->u), o->v);
	*mn = vmin3(*mn, c);
	*mx = vmax3(*mx, c);
	c = make_float3(1e-4f, 1e-4f, 1e-4f);
	*mn = sub(*mn, c);
	*mx = add(*mx, c);
}

/*
** World-space AABB of one bounded primitive. The cylinder box is the
** conservative cube center +/- (radius + height/2): slightly loose for any
** axis, but always correct regardless of the cylinder's orientation.
*/
void	obj_bounds(t_object *o, cl_float3 *mn, cl_float3 *mx)
{
	float		r;
	cl_float3	pad;

	if (o->type == OBJ_QUAD)
	{
		quad_bounds(o, mn, mx);
		return ;
	}
	r = o->radius;
	if (o->type == OBJ_CYLINDER)
		r = o->radius + o->height * 0.5f;
	pad = make_float3(r, r, r);
	*mn = sub(o->center, pad);
	*mx = add(o->center, pad);
}

/* Combined AABB over prim[first .. first+count); box[0]=min, box[1]=max. */
void	bvh_range_bounds(t_data *d, int first, int count, cl_float3 box[2])
{
	cl_float3	mn;
	cl_float3	mx;
	int			i;

	obj_bounds(&d->objects[d->bvh.prim[first]], &box[0], &box[1]);
	i = first + 1;
	while (i < first + count)
	{
		obj_bounds(&d->objects[d->bvh.prim[i]], &mn, &mx);
		box[0] = vmin3(box[0], mn);
		box[1] = vmax3(box[1], mx);
		i++;
	}
}
