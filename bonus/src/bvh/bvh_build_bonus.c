/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_build_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 23:55:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 23:55:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/* Centroid of one object's bounding box along one axis (0=x, 1=y, 2=z). */
static float	centroid_axis(t_data *d, int obj_idx, int axis)
{
	cl_float3	mn;
	cl_float3	mx;

	obj_bounds(&d->objects[obj_idx], &mn, &mx);
	return ((mn.s[axis] + mx.s[axis]) * 0.5f);
}

/*
** Midpoint split: move every prim whose centroid is left of the range's
** spatial middle to the front. Falls back to a plain median split when all
** centroids land on one side, so the tree depth stays ~log2(n).
*/
static int	partition_prims(t_data *d, int first, int count, int axis)
{
	cl_float3	box[2];
	float		split;
	int			i;
	int			mid;
	int			tmp;

	bvh_range_bounds(d, first, count, box);
	split = (box[0].s[axis] + box[1].s[axis]) * 0.5f;
	mid = first;
	i = first;
	while (i < first + count)
	{
		if (centroid_axis(d, d->bvh.prim[i], axis) < split)
		{
			tmp = d->bvh.prim[i];
			d->bvh.prim[i] = d->bvh.prim[mid];
			d->bvh.prim[mid] = tmp;
			mid++;
		}
		i++;
	}
	if (mid == first || mid == first + count)
		mid = first + count / 2;
	return (mid);
}

/* Recursively build the subtree for prim[first..first+count); returns idx. */
static int	build_node(t_data *d, int first, int count)
{
	cl_float3	box[2];
	cl_float3	ext;
	int			idx;
	int			axis;
	int			mid;

	bvh_range_bounds(d, first, count, box);
	idx = d->bvh.nnodes++;
	d->bvh.nodes[idx].bmin = box[0];
	d->bvh.nodes[idx].bmax = box[1];
	d->bvh.nodes[idx].left = first;
	d->bvh.nodes[idx].right = 0;
	d->bvh.nodes[idx].count = count;
	if (count <= 4)
		return (idx);
	ext = sub(box[1], box[0]);
	axis = (ext.y > ext.x);
	if (ext.s[2] > ext.s[axis])
		axis = 2;
	mid = partition_prims(d, first, count, axis);
	d->bvh.nodes[idx].count = 0;
	d->bvh.nodes[idx].left = build_node(d, first, mid - first);
	d->bvh.nodes[idx].right = build_node(d, mid, first + count - mid);
	return (idx);
}

/*
** One-time setup: split object indices into BVH prims (front) and planes
** (tail). obj_count is fixed after parsing, so capacities never change.
*/
static void	bvh_alloc(t_data *d)
{
	uint32_t	i;
	int			tail;

	d->bvh.prim = (int *)malloc(sizeof(int) * (d->obj_count + 1));
	check_null(d->bvh.prim);
	d->bvh.nprim = 0;
	tail = (int)d->obj_count;
	i = 0;
	while (i < d->obj_count)
	{
		if (d->objects[i].type == OBJ_PLANE)
			d->bvh.prim[--tail] = (int)i;
		else
			d->bvh.prim[d->bvh.nprim++] = (int)i;
		i++;
	}
	d->bvh.plane_count = (int)d->obj_count - d->bvh.nprim;
	d->bvh.nodes = (t_bvh_node *)malloc(sizeof(t_bvh_node)
			* (2 * d->bvh.nprim + 1));
	check_null(d->bvh.nodes);
}

/* (Re)build the flat tree. Cheap (~microseconds) at this scene size. */
void	bvh_build(t_data *data)
{
	if (!data->bvh.nodes)
		bvh_alloc(data);
	data->bvh.nnodes = 0;
	if (data->bvh.nprim > 0)
		build_node(data, 0, data->bvh.nprim);
}
