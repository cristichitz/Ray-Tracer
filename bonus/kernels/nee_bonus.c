/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nee_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 19:01:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

#define NEE_PI 3.14159265358979f

float	sample_light(__constant t_object *light, uint *seed, float3 *q,
		float3 *nl)
{
	float3	dir;

	if (light->type == OBJ_SPHERE)
	{
		dir = random_unit_vector(seed);
		*q = light->center + dir * light->radius;
		*nl = dir;
		return (4.0f * NEE_PI * light->radius * light->radius);
	}
	*q = light->center + light->u * random_float(0.0f, 1.0f, seed) + light->v
		* random_float(0.0f, 1.0f, seed);
	*nl = light->normal;
	return (length(cross(light->u, light->v)));
}

static int	pick_emitter(t_scene sc, uint *seed, int *n)
{
	int		idx;
	int		i;

	*n = 0;
	idx = -1;
	i = 0;
	while (i < sc.count)
	{
		if (sc.objs[i].material.type == 2 && (sc.objs[i].type == OBJ_QUAD
				|| sc.objs[i].type == OBJ_SPHERE))
		{
			(*n)++;
			if (random_float(0.0f, 1.0f, seed) * (float)(*n) < 1.0f)
				idx = i;
		}
		i++;
	}
	return (idx);
}

/*
** Geometric contribution: cosine-weighted area / clamped inverse-square.
** The clamp on dist2 prevents firefly pixels from points near the light.
*/
static float3	nee_shade(__constant t_object *lt, float3 n,
				float3 nl, float3 wl)
{
	float	dist2;
	float	cos_surf;
	float	cos_light;
	float	area;

	dist2 = dot(wl, wl);
	wl = wl / sqrt(dist2);
	cos_surf = dot(n, wl);
	cos_light = fabs(dot(nl, wl));
	if (cos_surf <= 0.0f || cos_light <= 1e-6f)
		return ((float3)(0.0f));
	area = length(cross(lt->u, lt->v));
	if (lt->type == OBJ_SPHERE)
		area = 4.0f * NEE_PI * lt->radius * lt->radius;
	return (lt->material.albedo * (cos_surf * cos_light
			* area / (NEE_PI * fmax(dist2, 0.25f))));
}

static float3	nee_eval(t_scene sc, t_hit_record rec,
				__constant t_object *light, uint *seed)
{
	float3			q;
	float3			nl;
	float3			wl;
	t_hit_record	tmp;

	sample_light(light, seed, &q, &nl);
	wl = q - rec.p;
	if (dot(rec.normal, normalize(wl)) <= 0.0f)
		return ((float3)(0.0f));
	if (hit_objects(sc, make_ray(rec.p, normalize(wl)),
			interval_init(0.001f, length(wl) - 0.001f), &tmp))
		return ((float3)(0.0f));
	return (nee_shade(light, rec.normal, nl, wl));
}

float3	direct_light(t_scene sc, t_hit_record rec, uint *seed)
{
	int					idx;
	int					n;
	__constant t_object	*light;

	idx = pick_emitter(sc, seed, &n);
	if (idx < 0)
		return ((float3)(0.0f));
	light = &sc.objs[idx];
	return (nee_eval(sc, rec, light, seed) * (float)n);
}
