/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:55:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/16 11:50:19 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"
#include <stdlib.h>

static void	get_sphere_uv(const t_vec3 p, float *u, float *v)
{
	float	theta;
	float	phi;

	theta = acosf(-p.y);
	phi = atan2f(-p.z, p.x) + M_PI;
	*u = phi / (2 * M_PI);
	*v = theta / M_PI;
}

static float	get_sphere_root(t_sphere_var *v, t_interval *t)
{
	v->discriminant = v->h * v->h - v->a * v->c;
	if (v->discriminant < 0)
		return (-1);
	v->sqrtd = sqrtf(v->discriminant);
	v->root = (v->h - v->sqrtd) / v->a;
	if (!t->surrounds(t, v->root))
	{
		v->root = (v->h + v->sqrtd) / v->a;
		if (!t->surrounds(t, v->root))
			return (-1);
	}
	return (v->root);
}

bool	hit_sphere(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_sphere		*self;
	t_vec3			oc;
	t_sphere_var	v;
	t_vec3			outward_normal;

	self = (t_sphere *)base;
	oc = sub(self->center, ray.origin);
	v.a = dot(ray.dir, ray.dir);
	v.h = dot(ray.dir, oc);
	v.c = dot(oc, oc) - self->radius * self->radius;
	if (get_sphere_root(&v, &t) < 0)
		return (false);
	rec->t = v.root;
	rec->p = ray.at(&ray, rec->t);
	rec->mat = self->mat;
	outward_normal = divide(sub(rec->p, self->center), self->radius);
	rec->set_face_normal(rec, ray, outward_normal);
	get_sphere_uv(outward_normal, &rec->u, &rec->v);
	return (true);
}

void	resize_sphere(void *base, float scalar)
{
	t_sphere	*self;

	self = (t_sphere *)base;
	self->radius += scalar;
	if (self->radius < 0.01f)
		self->radius = 0.01f;
}

t_sphere	*make_sphere(t_sphere sphere)
{
	t_sphere	*s;

	s = malloc(sizeof(t_sphere));
	if (!s)
		return (NULL);
	*s = sphere;
	s->base.hit = hit_sphere;
	s->base.resize = resize_sphere;
	s->base.rotate = NULL;
	s->base.destroy = NULL;
	s->base.material = material_sphere;
	return (s);
}
