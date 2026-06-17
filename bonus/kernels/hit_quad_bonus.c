/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_quad_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:20 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:40:59 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

bool	is_interior(float a, float b, t_hit_record *rec)
{
	t_interval	ui;

	ui = interval_init(0.0f, 1.0f);
	if (!interval_contains(&ui, a) || !interval_contains(&ui, b))
		return (false);
	rec->u = a;
	rec->v = b;
	return (true);
}

bool	quad_intersection(t_object *self, float3 intersection,
		t_hit_record *rec)
{
	float3	p;
	float	alpha;
	float	beta;

	p = intersection - self->center;
	alpha = dot(self->w, cross(p, self->v));
	beta = dot(self->w, cross(self->u, p));
	if (!is_interior(alpha, beta, rec))
		return (false);
	rec->p = intersection;
	rec->mat = self->material;
	return (true);
}

bool	hit_quad(t_object self, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	float	denom;
	float	t;
	float3	intersection;

	denom = dot(self.normal, ray.dir);
	if (fabs(denom) < 1e-8f)
		return (false);
	t = (self.d - dot(self.normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return (false);
	intersection = ray_at(&ray, t);
	if (!quad_intersection(&self, intersection, rec))
		return (false);
	rec->t = t;
	ft_set_face_normal(rec, ray, self.normal);
	return (true);
}
