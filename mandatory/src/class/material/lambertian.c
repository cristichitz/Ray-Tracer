/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lambertian.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:39:27 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 20:41:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"

bool	scatter_lambertian(t_material *self, t_ray r_in, t_hit_record rec,
		t_scat *scat)
{
	t_vec3	scatter_direction;

	(void)r_in;
	scatter_direction = add(rec.normal, random_unit_vector());
	if (near_zero(scatter_direction))
		scatter_direction = rec.normal;
	scat->scattered = make_ray(rec.p, scatter_direction);
	scat->attenuation = self->tex.albedo;
	return (true);
}

t_vec3	emitted_nothing_lam(t_material *self, float u, float v, t_vec3 p)
{
	(void)self;
	(void)u;
	(void)v;
	(void)p;
	return (make_vec(0.0f, 0.0f, 0.0f));
}

t_material	init_lambertian(t_vec3 col)
{
	t_material	new_mat;

	new_mat.tex = init_solid_color(col);
	new_mat.scatter = scatter_lambertian;
	new_mat.emitted = emitted_nothing_lam;
	return (new_mat);
}
