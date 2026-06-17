/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_body.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 19:52:19 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 19:59:24 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

static void	init_cyl_var(t_cylinder *self, t_ray ray, t_cyl_var *v)
{
	v->w = sub(ray.origin, self->center);
	v->dv = dot(ray.dir, self->normal);
	v->wv = dot(v->w, self->normal);
	v->a = dot(ray.dir, ray.dir) - v->dv * v->dv;
	v->half_b = dot(ray.dir, v->w) - v->dv * v->wv;
	v->c = dot(v->w, v->w) - v->wv * v->wv - self->radius * self->radius;
}

static float	get_cyl_root(t_cylinder *self, t_cyl_var *v, t_interval *t)
{
	float	half_h;

	half_h = self->height / 2;
	v->discriminant = v->half_b * v->half_b - v->a * v->c;
	if (v->discriminant < 0)
		return (-1);
	v->sqrtd = sqrtf(v->discriminant);
	v->root = (-v->half_b - v->sqrtd) / v->a;
	v->m = v->wv + v->root * v->dv;
	if (!t->surrounds(t, v->root) || v->m < -half_h || v->m > half_h)
	{
		v->root = (-v->half_b + v->sqrtd) / v->a;
		v->m = v->wv + v->root * v->dv;
	}
	if (t->surrounds(t, v->root) && v->m >= -half_h && v->m <= half_h)
		return (v->root);
	return (-1);
}

bool	hit_cyl_body(t_cylinder *self, t_ray ray, t_interval t,
		t_hit_record *rec)
{
	t_cyl_var	v;
	t_vec3		outward_normal;

	init_cyl_var(self, ray, &v);
	if (get_cyl_root(self, &v, &t) < 0)
		return (false);
	rec->t = v.root;
	rec->p = ray.at(&ray, v.root);
	outward_normal = norm(sub(sub(rec->p, self->center), scale(self->normal,
					v.m)));
	rec->set_face_normal(rec, ray, outward_normal);
	rec->mat = self->mat;
	return (true);
}
