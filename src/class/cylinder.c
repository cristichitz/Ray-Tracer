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
#include "plane.h"

// TODO: hollow cyclinder... Should be a closed cyclinder.

bool hit_circle(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_cylinder *self;
	float denom;
	float t;
	t_vec3 intersec;
	t_vec3 c;
	t_vec3 to_hit;

	self = (t_cylinder *)base;

	c = add(self->center, scale(self->normal, self->height / 2.0f));

	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);

	t = (self->D_top - dot(self->normal, ray.origin)) / denom;

	if (!ray_t.contains(&ray_t, t))
		return (false);
	
	intersec = ray.at(&ray, t);

	to_hit = sub(intersec, c);

	if (dot(to_hit, to_hit) > (self->radius * self->radius))
		return (false);
	rec->p = intersec;
	rec->t = t;

	rec->mat = init_lambertian(make_vec(0.0, 0.0, 1.0));
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

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
	dv = dot(ray.dir, self->normal);
	wv = dot(w, self->normal);

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
	if (hit_circle(base, ray, t, rec))
		return (true);
	rec->t = root;
	rec->p = ray.at(&ray, rec->t);
	outward_normal = norm(sub(sub(rec->p, self->center), scale(self->normal, m)));
	rec->set_face_normal(rec, ray, outward_normal);
	rec->mat = self->mat;
	return (true);
}

t_cylinder *make_cylinder(t_cylinder cylinder)
{
	t_cylinder *c;
	float D;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	c->base.hit = hit_cylinder;
	D = dot(c->normal, c->center);

	c->D_top = D + (c->height / 2.0f);
	return (c);
}