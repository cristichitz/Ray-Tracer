/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:54:58 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:55:04 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"

//TODO: too many params
bool	scatter_lambertian(t_material *self, t_ray r_in, t_hit_record rec,
		t_vec3 *attenuation, t_ray *scattered)
{
	t_vec3	scatter_direction;

	(void)r_in;
	scatter_direction = add(rec.normal, random_unit_vector());
	if (near_zero(scatter_direction))
		scatter_direction = rec.normal;
	*scattered = make_ray(rec.p, scatter_direction);
	*attenuation = self->tex.albedo;
	return (true);
}

t_vec3	reflect(t_vec3 v, t_vec3 n)
{
	t_vec3	b;

	b = scale(n, 2.0f * dot(v, n));
	return (sub(v, b));
}

//TODO: too many params
bool	scatter_metal(t_material *self, t_ray r_in, t_hit_record rec,
		t_vec3 *attenuation, t_ray *scattered)
{
	t_vec3	reflected;

	reflected = reflect(r_in.dir, rec.normal);
	*scattered = make_ray(rec.p, reflected);
	*attenuation = self->tex.albedo;
	return (true);
}

//TODO: too many params
bool	scatter_light(t_material *self, t_ray r_in, t_hit_record rec,
		t_vec3 *attenuation, t_ray *scattered)
{
	(void)self;
	(void)r_in;
	(void)rec;
	(void)attenuation;
	(void)scattered;
	return (false);
}

t_vec3	emitted_light(t_material *self, float u, float v, t_vec3 p)
{
	return (self->tex.value(&self->tex, u, v, p));
}

t_vec3	emitted_nothing(t_material *self, float u, float v, t_vec3 p)
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
	new_mat.emitted = emitted_nothing;
	return (new_mat);
}

t_material	init_metal(t_vec3 col)
{
	t_material	new_mat;

	new_mat.tex = init_solid_color(col);
	new_mat.scatter = scatter_metal;
	new_mat.emitted = emitted_nothing;
	return (new_mat);
}

t_material	init_diffuse_light(t_vec3 col)
{
	t_material new_mat;

	new_mat.tex = init_solid_color(col);
	new_mat.scatter = scatter_light;
	new_mat.emitted = emitted_light;

	return (new_mat);
}