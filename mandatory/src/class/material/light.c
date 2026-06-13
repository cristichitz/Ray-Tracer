/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:54:58 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 20:43:11 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable.h"

bool	scatter_light(t_material *self, t_ray r_in, t_hit_record rec,
		t_scat *scat)
{
	(void)self;
	(void)r_in;
	(void)rec;
	(void)scat;
	return (false);
}

t_vec3	emitted_light(t_material *self, float u, float v, t_vec3 p)
{
	return (self->tex.value(&self->tex, u, v, p));
}

t_material	init_diffuse_light(t_vec3 col)
{
	t_material	new_mat;

	new_mat.tex = init_solid_color(col);
	new_mat.scatter = scatter_light;
	new_mat.emitted = emitted_light;
	return (new_mat);
}
