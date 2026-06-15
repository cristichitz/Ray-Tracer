/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   portal_hit_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/15 12:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/* Host ray/plane: parametric t of the hit (or -1), normal written to *n. */
static float	hit_plane_h(t_object *o, t_ray ray, cl_float3 *n)
{
	float	denom;

	denom = dot(o->normal, ray.dir);
	if (fabsf(denom) < 1e-6f)
		return (-1.0f);
	*n = o->normal;
	return ((o->d - dot(o->normal, ray.origin)) / denom);
}

/* Host ray/quad: bounded by the same alpha/beta in-[0,1] test as the kernel. */
static float	hit_quad_h(t_object *o, t_ray ray, cl_float3 *n)
{
	float		denom;
	float		t;
	cl_float3	p;
	float		a;
	float		b;

	denom = dot(o->normal, ray.dir);
	if (fabsf(denom) < 1e-6f)
		return (-1.0f);
	t = (o->d - dot(o->normal, ray.origin)) / denom;
	if (t < 0.0f)
		return (-1.0f);
	p = sub(add(ray.origin, scale(ray.dir, t)), o->center);
	a = dot(o->w, cross(p, o->v));
	b = dot(o->w, cross(o->u, p));
	if (a < 0.0f || a > 1.0f || b < 0.0f || b > 1.0f)
		return (-1.0f);
	*n = o->normal;
	return (t);
}

/* Only static walls (planes / quads, non-dynamic, non-portal) take a portal. */
static int	skip_surface(t_object *o)
{
	if (o->material.dynamic || o->material.portal >= 0)
		return (1);
	if (o->type != OBJ_PLANE && o->type != OBJ_QUAD)
		return (1);
	return (0);
}

static float	surf_t(t_object *o, t_ray ray, cl_float3 *n)
{
	if (o->type == OBJ_PLANE)
		return (hit_plane_h(o, ray, n));
	return (hit_quad_h(o, ray, n));
}

/*
** Closest static surface a ray strikes, returned as hit point *p and a normal
** *n flipped to face the ray. 1 on a hit, 0 if the ray escapes the scene.
*/
int	scene_ray_hit(t_data *data, t_ray ray, cl_float3 *p, cl_float3 *n)
{
	float		best;
	float		t;
	cl_float3	nn;
	uint32_t	i;
	int			hit;

	best = 1e30f;
	hit = 0;
	i = 0;
	while (i < data->obj_count)
	{
		t = -1.0f;
		if (!skip_surface(&data->objects[i]))
			t = surf_t(&data->objects[i], ray, &nn);
		if (t > 1e-3f && t < best)
		{
			best = t;
			*n = nn;
			hit = 1;
		}
		i++;
	}
	if (hit && dot(*n, ray.dir) > 0.0f)
		*n = scale(*n, -1.0f);
	if (hit)
		*p = add(ray.origin, scale(ray.dir, best));
	return (hit);
}
