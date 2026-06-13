/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_caps.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 19:52:19 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 19:52:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

static t_cap	make_cap(t_cylinder *self, float offset)
{
	t_cap	cap;

	cap.center = add(self->center, scale(self->normal, offset));
	cap.normal = self->normal;
	cap.radius = self->radius;
	cap.mat = self->mat;
	return (cap);
}

static bool	hit_cap(t_cap cap, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	float	denom;
	float	t;
	float	cap_d;
	t_vec3	to_hit;

	cap_d = dot(cap.normal, cap.center);
	denom = dot(cap.normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (cap_d - dot(cap.normal, ray.origin)) / denom;
	if (!ray_t.contains(&ray_t, t))
		return (false);
	to_hit = sub(ray.at(&ray, t), cap.center);
	if (dot(to_hit, to_hit) > (cap.radius * cap.radius))
		return (false);
	rec->p = ray.at(&ray, t);
	rec->t = t;
	rec->mat = cap.mat;
	rec->set_face_normal(rec, ray, cap.normal);
	return (true);
}

bool	hit_caps(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_cylinder		*self;
	t_hit_record	tmp;
	bool			hit_any;
	float			half_h;

	self = (t_cylinder *)base;
	tmp.set_face_normal = ft_set_face_normal;
	hit_any = false;
	half_h = self->height / 2.0f;
	if (hit_cap(make_cap(self, half_h), ray, ray_t, &tmp))
	{
		*rec = tmp;
		ray_t.max = tmp.t;
		hit_any = true;
	}
	if (hit_cap(make_cap(self, -half_h), ray, ray_t, &tmp))
	{
		*rec = tmp;
		hit_any = true;
	}
	return (hit_any);
}
