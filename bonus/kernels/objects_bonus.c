/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   objects_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:01:32 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:01:47 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static bool	hit_one(t_object obj, t_ray ray, t_interval span, t_hit_record *rec)
{
	if (obj.type == OBJ_SPHERE)
		return (hit_sphere(obj, ray, span, rec));
	if (obj.type == OBJ_PLANE)
		return (hit_plane(obj, ray, span, rec));
	if (obj.type == OBJ_CYLINDER)
		return (hit_cylinder(obj, ray, span, rec));
	if (obj.type == OBJ_QUAD)
		return (hit_quad(obj, ray, span, rec));
	return (false);
}

static float	safe_inv(float d)
{
	if (fabs(d) < 1e-12f)
		return (copysign(1e12f, d));
	return (1.0f / d);
}

static bool	hit_aabb(float3 bmin, float3 bmax, t_ray ray, float tmin,
		float tmax)
{
	float3	inv;
	float3	t0;
	float3	t1;
	float3	lo;
	float3	hi;
	float	tn;
	float	tf;

	inv = (float3)(safe_inv(ray.dir.x), safe_inv(ray.dir.y),
			safe_inv(ray.dir.z));
	t0 = (bmin - ray.origin) * inv;
	t1 = (bmax - ray.origin) * inv;
	lo = fmin(t0, t1);
	hi = fmax(t0, t1);
	tn = fmax(fmax(lo.x, lo.y), fmax(lo.z, tmin));
	tf = fmin(fmin(hi.x, hi.y), fmin(hi.z, tmax));
	return (tn <= tf);
}

static bool	hit_range(t_scene sc, int first, int count, t_ray ray, float tmin,
		float *closest, t_hit_record *rec)
{
	t_hit_record	temp;
	bool			hit_anything;
	int				i;
	t_object		obj;

	hit_anything = false;
	i = 0;
	while (i < count)
	{
		obj = sc.objs[sc.prim[first + i]];
		if (hit_one(obj, ray, interval_init(tmin, *closest), &temp))
		{
			temp.obj_type = obj.type;
			hit_anything = true;
			*closest = temp.t;
			*rec = temp;
		}
		i++;
	}
	return (hit_anything);
}

bool	hit_objects(t_scene sc, t_ray ray, t_interval t, t_hit_record *rec)
{
	int			stack[32];
	int			sp;
	float		closest;
	bool		hit_anything;
	t_bvh_node	node;

	sp = 0;
	closest = t.max;
	hit_anything = hit_range(sc, sc.plane_first, sc.plane_count, ray, t.min,
			&closest, rec);
	if (sc.nnodes > 0)
		stack[sp++] = 0;
	while (sp > 0)
	{
		node = sc.nodes[stack[--sp]];
		if (!hit_aabb(node.bmin, node.bmax, ray, t.min, closest))
			continue ;
		if (node.count > 0)
		{
			if (hit_range(sc, node.left, node.count, ray, t.min, &closest, rec))
				hit_anything = true;
		}
		else if (sp < 30)
		{
			stack[sp++] = node.left;
			stack[sp++] = node.right;
		}
	}
	return (hit_anything);
}
