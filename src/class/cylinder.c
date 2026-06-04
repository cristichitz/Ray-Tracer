/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 12:13:26 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

// TODO: hollow cyclinder... Should be a closed cyclinder.
bool hit_cylinder(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_cylinder *self;
	t_vec3 w;

	float dv;
	float wv;
	float a;
	float half_b;
	float c;
	float m;

	float discriminant;
	float sqrtd;
	float root;
	t_vec3 outward_normal;

	self = (t_cylinder *)base;
	w = sub(ray.origin, self->center);
	dv = dot(ray.dir, self->uvec);
	wv = dot(w, self->uvec);

	a = dot(ray.dir, ray.dir) - dv * dv;
	half_b = dot(ray.dir, w) - dv * wv;
	c = dot(w, w) - wv *wv - self->radius * self->radius;
	
	discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return (false);
	
	sqrtd = sqrt(discriminant);
	
	root = (-half_b - sqrtd) / a;
	m  = wv + root *dv;
	if(!t.surrounds(&t, root) || m < -self->height / 2 || m > self->height / 2)
	{
		root = (-half_b + sqrtd) / a;
		m = wv + root *dv;
		if (!t.surrounds(&t, root) || m < -self->height / 2 || m > self->height / 2)
			return (false);
	}
	rec->t = root;
	rec->p = ray.at(&ray, rec->t);
	outward_normal = norm(sub(sub(rec->p, self->center), scale(self->uvec, m)));
	rec->set_face_normal(rec, ray, outward_normal);
	rec->colour =  make_vec(self->colour.r, self->colour.g, self->colour.b);
	return (true);
}

t_cylinder *make_cylinder(t_cylinder cylinder)
{
	t_cylinder *c;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	c->base.hit = hit_cylinder;
	return (c);
}