/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portal_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/15 12:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Append the two portal ellipses up front (parked far below the scene, marked
** inactive) so the GPU buffers are sized once and a click only re-poses an
** existing object instead of growing the array.
*/
void	reserve_portals(t_data *data)
{
	t_material	m;
	t_object	e;
	int			i;

	m = material_init(make_float3(0.10f, 0.45f, 1.0f), 0);
	e = make_obj_ellipse(make_float3(0.0f, -1.0e6f, 0.0f),
			make_float3(1.0f, 0.0f, 0.0f), make_float3(0.0f, 0.0f, 1.0f), m);
	i = 0;
	while (i < 2)
	{
		data->portal_obj[i] = (int)data->obj_count;
		data->portal_active[i] = 0;
		add_object(data, e);
		i++;
	}
	data->portal_next = 0;
}

/* Push the two portal object indices (or -1 when inactive) to the kernel. */
void	set_portal_args(t_data *data)
{
	cl_int	p0;
	cl_int	p1;

	p0 = -1;
	p1 = -1;
	if (data->portal_active[0])
		p0 = data->portal_obj[0];
	if (data->portal_active[1])
		p1 = data->portal_obj[1];
	clSetKernelArg(data->gpu.kernel, 11, sizeof(cl_int), &p0);
	clSetKernelArg(data->gpu.kernel, 12, sizeof(cl_int), &p1);
}

/*
** Clamp a wall portal's height so its oval never sinks below the floor: keep
** the clicked height, but if it is so low the base would clip through the
** ground, lift it just enough to rest on it. Frees vertical placement while
** still letting a low click "sit" the portal on the floor like a doorway.
*/
static float	portal_floor_snap(t_data *data, float y)
{
	float	h;
	int		i;

	h = -1e9f;
	i = 0;
	while (i < data->phys.collider_count)
	{
		if (data->phys.colliders[i].n.y > 0.9f && data->phys.colliders[i].d > h)
			h = data->phys.colliders[i].d;
		i++;
	}
	if (h > -1e8f && y < h + PORTAL_RY)
		return (h + PORTAL_RY);
	return (y);
}

/* Tint + tag a portal object so an unpaired one still reads as blue / orange. */
static void	portal_paint(t_object *o, int slot)
{
	if (slot == 0)
		o->material.albedo = make_float3(0.10f, 0.45f, 1.0f);
	else
		o->material.albedo = make_float3(1.0f, 0.45f, 0.05f);
	o->material.portal = slot;
}

/*
** Re-pose portal `slot` as an upright oval on the surface that was hit. On a
** wall (near-horizontal normal) its base is snapped to the floor so it looks
** like a doorway; u is the horizontal semi-axis, v the (taller) vertical one.
*/
static void	build_portal(t_data *data, int slot, cl_float3 p, cl_float3 n)
{
	t_object	*o;
	cl_float3	up;
	cl_float3	c;

	up = make_float3(0.0f, 1.0f, 0.0f);
	if (fabsf(dot(n, up)) > 0.95f)
		up = make_float3(1.0f, 0.0f, 0.0f);
	c = add(p, scale(n, PORTAL_OFFSET));
	if (fabsf(n.y) < 0.5f)
		c.y = portal_floor_snap(data, c.y);
	o = &data->objects[data->portal_obj[slot]];
	*o = make_obj_ellipse(c, scale(norm(cross(up, n)), PORTAL_RX),
			scale(norm(cross(n, cross(up, n))), PORTAL_RY), o->material);
	portal_paint(o, slot);
	data->portal_active[slot] = 1;
}

/*
** Left click: drop a portal on the static surface under the cursor. Two live
** at once; the third placement recycles the oldest (FIFO ring of 2).
*/
void	place_portal(t_data *data, t_ray ray)
{
	cl_float3	p;
	cl_float3	n;
	int			slot;

	if (!scene_ray_hit(data, ray, &p, &n))
		return ;
	slot = data->portal_next;
	build_portal(data, slot, p, n);
	data->portal_next = 1 - slot;
	data->scene_dirty = 1;
	data->frame_index = 0;
}
