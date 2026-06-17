/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:01:32 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 19:12:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static bool	hit_one(__constant t_object *obj, t_ray ray,
			t_interval span, t_hit_record *rec)
{
	if (obj->type == OBJ_SPHERE)
		return (hit_sphere(obj, ray, span, rec));
	if (obj->type == OBJ_PLANE)
		return (hit_plane(obj, ray, span, rec));
	if (obj->type == OBJ_CYLINDER)
		return (hit_cylinder(obj, ray, span, rec));
	if (obj->type == OBJ_QUAD)
		return (hit_quad(obj, ray, span, rec));
	return (false);
}

static bool	hit_aabb(float3 bmin, float3 bmax,
			t_ray ray, t_interval span)
{
	float3	inv;
	float3	lo;
	float3	hi;

	inv = (float3)(safe_inv(ray.dir.x), safe_inv(ray.dir.y),
			safe_inv(ray.dir.z));
	lo = fmin((bmin - ray.origin) * inv,
			(bmax - ray.origin) * inv);
	hi = fmax((bmin - ray.origin) * inv,
			(bmax - ray.origin) * inv);
	return (fmax(fmax(lo.x, lo.y), fmax(lo.z, span.min))
		<= fmin(fmin(hi.x, hi.y), fmin(hi.z, span.max)));
}

static bool	hit_range(t_scene sc, t_trace *tr,
			int first, int count)
{
	t_hit_record		temp;
	bool				hit_anything;
	int					i;
	__constant t_object	*obj;

	hit_anything = false;
	i = 0;
	while (i < count)
	{
		obj = &sc.objs[sc.prim[first + i]];
		if (hit_one(obj, tr->ray,
				interval_init(tr->tmin, tr->closest), &temp))
		{
			temp.obj_type = obj->type;
			hit_anything = true;
			tr->closest = temp.t;
			tr->rec = temp;
		}
		i++;
	}
	return (hit_anything);
}

static bool	bvh_walk(t_scene sc, t_trace *tr, int *stack, int sp)
{
	bool		any;
	t_bvh_node	node;

	any = false;
	while (sp > 0)
	{
		node = sc.nodes[stack[--sp]];
		if (!hit_aabb(node.bmin, node.bmax, tr->ray,
				interval_init(tr->tmin, tr->closest)))
			continue ;
		if (node.count > 0)
		{
			if (hit_range(sc, tr, node.left, node.count))
				any = true;
		}
		else if (sp < 30)
		{
			stack[sp++] = node.left;
			stack[sp++] = node.right;
		}
	}
	return (any);
}

bool	hit_objects(t_scene sc, t_ray ray, t_interval t,
		t_hit_record *rec)
{
	int		stack[32];
	t_trace	tr;
	bool	hit;

	tr.ray = ray;
	tr.tmin = t.min;
	tr.closest = t.max;
	hit = hit_range(sc, &tr, sc.plane_first, sc.plane_count);
	if (sc.nnodes > 0)
	{
		stack[0] = 0;
		if (bvh_walk(sc, &tr, stack, 1))
			hit = true;
	}
	if (hit)
		*rec = tr.rec;
	return (hit);
}
