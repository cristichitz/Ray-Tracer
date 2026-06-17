/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:50:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:50:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

/* Russian-roulette path termination once a path is dim; returns 1 to stop. */
static bool	roulette(t_path *pt, uint *seed)
{
	float	q;

	q = fmax(pt->throughput.x, fmax(pt->throughput.y, pt->throughput.z));
	if (random_float(0.0f, 1.0f, seed) > q)
		return (true);
	pt->throughput /= q;
	return (false);
}

/*
** Shade one surface hit and set up the next bounce. Quad AND sphere lights are
** handled by NEE (see is_nee_light), so their emission is only counted on the
** camera ray / after a specular bounce to avoid double counting. Returns 0 on
** a non-scattering material (the path ends).
*/
static bool	shade_hit(t_scene sc, t_hit_record *rec, t_path *pt, uint *seed)
{
	float3	attenuation;
	t_ray	scattered;

	if (pt->count_emission || rec->mat.type != 2 || (rec->obj_type != OBJ_QUAD
			&& rec->obj_type != OBJ_SPHERE))
		pt->out += pt->throughput * emitted(&rec->mat);
	if (rec->mat.type == 1)
	{
		scatter_metal(&rec->mat, pt->ray, *rec, &attenuation, &scattered);
		pt->count_emission = true;
	}
	else if (rec->mat.type == 0)
	{
		pt->out += pt->throughput * rec->mat.albedo
			* direct_light(sc, *rec, seed);
		scatter_lambertian(&rec->mat, *rec, &attenuation, &scattered, seed);
		pt->count_emission = false;
	}
	else
		return (false);
	pt->throughput *= attenuation;
	pt->ray = scattered;
	return (true);
}

float3	get_ray_color(t_scene sc, t_ray ray, t_image *f, uint *seed)
{
	t_path			pt;
	t_hit_record	rec;
	int				depth;

	pt.ray = ray;
	pt.throughput = (float3)(1.0f);
	pt.out = (float3)(0.0f);
	pt.count_emission = true;
	depth = 0;
	while (depth < f->max_depth)
	{
		if (!hit_objects(sc, pt.ray, interval_init(0.001f, INFINITY), &rec))
			return (pt.out + pt.throughput * f->background);
		if (!shade_hit(sc, &rec, &pt, seed))
			break ;
		if (depth > 3 && roulette(&pt, seed))
			break ;
		depth++;
	}
	return (pt.out);
}

void	write_color(__global int *img_buffer, int id, float3 color)
{
	t_interval		intensity;
	__global uchar4	*ptr;
	uchar4			c;

	intensity = interval_init(0.0f, 0.999f);
	ptr = (__global uchar4 *)img_buffer;
	c.x = (uchar)(256 * interval_clamp(&intensity, color.x));
	c.y = (uchar)(256 * interval_clamp(&intensity, color.y));
	c.z = (uchar)(256 * interval_clamp(&intensity, color.z));
	c.w = 255;
	ptr[id] = c;
}
