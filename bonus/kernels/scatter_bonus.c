/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scatter_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:05:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

/* v is the incident ray, 2*dot(v,n)*n is the component along the normal. */
float3	reflect(float3 v, float3 n)
{
	return (v - n * 2.0f * dot(v, n));
}

bool	scatter_lambertian(t_material *self, t_hit_record rec, t_scat *scat,
		uint *seed)
{
	float3	scatter_direction;

	scatter_direction = rec.normal + random_unit_vector(seed);
	if (near_zero(scatter_direction))
		scatter_direction = rec.normal;
	scat->scattered = make_ray(rec.p, scatter_direction);
	scat->attenuation = self->albedo;
	return (true);
}

bool	scatter_metal(t_material *self, t_ray r_in, t_hit_record rec,
		t_scat *scat)
{
	float3	reflected;

	reflected = reflect(r_in.dir, rec.normal);
	scat->scattered = make_ray(rec.p, reflected);
	scat->attenuation = self->albedo;
	return (true);
}

float3	emitted(t_material *self)
{
	if (self->type == 2)
		return (self->albedo);
	return ((float3)(0.0f));
}
