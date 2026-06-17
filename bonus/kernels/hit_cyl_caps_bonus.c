/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_cyl_caps_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 18:18:42 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 18:55:36 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static t_cap	make_cap(__constant t_object *self, float offset,
		float normal_mult)
{
	t_cap	cap;

	cap.center = self->center + (self->normal * offset);
	cap.normal = self->normal * normal_mult;
	cap.radius = self->radius;
	cap.mat = self->material;
	return (cap);
}

static bool	hit_cap(t_cap cap, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	float	denom;
	float	t;
	float	cap_d;
	float3	to_hit;

	cap_d = dot(cap.normal, cap.center);
	denom = dot(cap.normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (cap_d - dot(cap.normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return (false);
	to_hit = ray_at(&ray, t) - cap.center;
	if (dot(to_hit, to_hit) > (cap.radius * cap.radius))
		return (false);
	rec->p = ray_at(&ray, t);
	rec->t = t;
	rec->mat = cap.mat;
	ft_set_face_normal(rec, ray, cap.normal);
	return (true);
}

bool	hit_caps(__constant t_object *self, t_ray ray, t_interval ray_t,
		t_hit_record *rec)
{
	t_hit_record	tmp;
	bool			hit_any;
	float			half_h;

	hit_any = false;
	half_h = self->height / 2.0f;
	if (hit_cap(make_cap(self, half_h, 1.0f), ray, ray_t, &tmp))
	{
		*rec = tmp;
		ray_t.max = tmp.t;
		hit_any = true;
	}
	if (hit_cap(make_cap(self, -half_h, -1.0f), ray, ray_t, &tmp))
	{
		*rec = tmp;
		hit_any = true;
	}
	return (hit_any);
}
