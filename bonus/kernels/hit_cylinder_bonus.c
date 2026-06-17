/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_cylinder_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:27 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 18:54:14 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static void	init_cyl_var(__constant t_object *self, t_ray ray, t_cyl_var *v)
{
	v->w = ray.origin - self->center;
	v->dv = dot(ray.dir, self->normal);
	v->wv = dot(v->w, self->normal);
	v->a = dot(ray.dir, ray.dir) - v->dv * v->dv;
	v->half_b = dot(ray.dir, v->w) - v->dv * v->wv;
	v->c = dot(v->w, v->w) - v->wv * v->wv - self->radius * self->radius;
}

static float	get_cyl_root(__constant t_object *self, t_cyl_var *v,
		t_interval *t)
{
	float	half_h;

	half_h = self->height / 2;
	v->discriminant = v->half_b * v->half_b - v->a * v->c;
	if (v->discriminant < 0)
		return (-1);
	v->sqrtd = sqrt(v->discriminant);
	v->root = (-v->half_b - v->sqrtd) / v->a;
	v->m = v->wv + v->root * v->dv;
	if (!interval_surrounds(t, v->root) || v->m < -half_h || v->m > half_h)
	{
		v->root = (-v->half_b + v->sqrtd) / v->a;
		v->m = v->wv + v->root * v->dv;
	}
	if (interval_surrounds(t, v->root) && v->m >= -half_h && v->m <= half_h)
		return (v->root);
	return (-1);
}

bool	hit_cyl_body(__constant t_object *self, t_ray ray, t_interval t,
		t_hit_record *rec)
{
	t_cyl_var	v;
	float3		outward_normal;

	init_cyl_var(self, ray, &v);
	if (get_cyl_root(self, &v, &t) < 0)
		return (false);
	rec->t = v.root;
	rec->p = ray_at(&ray, v.root);
	outward_normal = normalize((rec->p - self->center) - self->normal * v.m);
	ft_set_face_normal(rec, ray, outward_normal);
	rec->mat = self->material;
	return (true);
}

bool	hit_cylinder(__constant t_object *self, t_ray ray, t_interval t,
		t_hit_record *rec)
{
	bool	hit_body;

	hit_body = hit_cyl_body(self, ray, t, rec);
	if (hit_body)
		t.max = rec->t;
	if (hit_caps(self, ray, t, rec))
		return (true);
	return (hit_body);
}
