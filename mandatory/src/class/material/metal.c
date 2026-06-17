/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metal.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:40:47 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 20:42:36 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"

t_vec3	emitted_nothing_met(t_material *self, float u, float v, t_vec3 p)
{
	(void)self;
	(void)u;
	(void)v;
	(void)p;
	return (make_vec(0.0f, 0.0f, 0.0f));
}

t_vec3	reflect(t_vec3 v, t_vec3 n)
{
	t_vec3	b;

	b = scale(n, 2.0f * dot(v, n));
	return (sub(v, b));
}

bool	scatter_metal(t_material *self, t_ray r_in, t_hit_record rec,
		t_scat *scat)
{
	t_vec3	reflected;

	reflected = reflect(r_in.dir, rec.normal);
	scat->scattered = make_ray(rec.p, reflected);
	scat->attenuation = self->tex.albedo;
	return (true);
}

t_material	init_metal(t_vec3 col)
{
	t_material	new_mat;

	new_mat.tex = init_solid_color(col);
	new_mat.scatter = scatter_metal;
	new_mat.emitted = emitted_nothing_met;
	new_mat.type = MAT_METAL;
	return (new_mat);
}
