/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/06/10 16:02:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "plane.h"
#include "quaternion.h"
#include <stdlib.h>

//TODO: too many params
static bool	hit_cap(t_cylinder *self, t_ray ray, t_interval ray_t,
		t_hit_record *rec, float offset)
{
	float	denom;
	float	t;
	float	cap_d;
	t_vec3	cap_center;
	t_vec3	to_hit;

	cap_center = add(self->center, scale(self->normal, offset));
	cap_d = dot(self->normal, cap_center);
	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (cap_d - dot(self->normal, ray.origin)) / denom;
	if (!ray_t.contains(&ray_t, t))
		return (false);
	to_hit = sub(ray.at(&ray, t), cap_center);
	if (dot(to_hit, to_hit) > (self->radius * self->radius))
		return (false);
	rec->p = ray.at(&ray, t);
	rec->t = t;
	rec->mat = self->mat;
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

static bool	hit_caps(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_cylinder		*self;
	t_hit_record	tmp;
	bool			hit_any;
	float			half_h;

	self = (t_cylinder *)base;
	tmp.set_face_normal = ft_set_face_normal;
	hit_any = false;
	half_h = self->height / 2.0f;
	if (hit_cap(self, ray, ray_t, &tmp, half_h))
	{
		*rec = tmp;
		ray_t.max = tmp.t;
		hit_any = true;
	}
	if (hit_cap(self, ray, ray_t, &tmp, -half_h))
	{
		*rec = tmp;
		hit_any = true;
	}
	return (hit_any);
}
//TODO: too many vars
bool	hit_cylinder(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_cylinder	*self;
	t_vec3		w;
	float		a;
	float		c;
	float		discriminant;
	float		root;
	float		sqrtd;
	float		dv;
	float		wv;
	float		half_b;
	float		m;
	t_vec3		outward_normal;
	bool		hit_body;

	self = (t_cylinder *)base;
	hit_body = false;
	w = sub(ray.origin, self->center);
	dv = dot(ray.dir, self->normal);
	wv = dot(w, self->normal);
	a = dot(ray.dir, ray.dir) - dv * dv;
	half_b = dot(ray.dir, w) - dv * wv;
	c = dot(w, w) - wv * wv - self->radius * self->radius;
	discriminant = half_b * half_b - a * c;
	if (discriminant >= 0)
	{
		sqrtd = sqrt(discriminant);
		root = (-half_b - sqrtd) / a;
		m = wv + root * dv;
		if (!t.surrounds(&t, root) || m < -self->height / 2 || m > self->height
			/ 2)
		{
			root = (-half_b + sqrtd) / a;
			m = wv + root * dv;
		}
		if (t.surrounds(&t, root) && m >= -self->height / 2 && m <= self->height
			/ 2)
		{
			rec->t = root;
			rec->p = ray.at(&ray, root);
			outward_normal = norm(sub(sub(rec->p, self->center),
						scale(self->normal, m)));
			rec->set_face_normal(rec, ray, outward_normal);
			rec->mat = self->mat;
			t.max = root;
			hit_body = true;
		}
	}
	if (hit_caps(base, ray, t, rec))
		return (true);
	return (hit_body);
}

void	resize_cylinder(void *base, float scalar)
{
	t_cylinder	*self;

	self = (t_cylinder *)base;
	self->height += scalar;
	if (self->height < 0.01f)
		self->height = 0.01f;
	self->radius += scalar;
	if (self->radius < 0.01f)
		self->radius = 0.01f;
}

void	rotate_cylinder(void *base, t_vec3 axis, float angle)
{
	t_cylinder		*self;
	t_quaternion	q;

	self = (t_cylinder *)base;
	q = make_quaternion(axis, angle);
	self->normal = norm(rotate_vec_by_quaternion(q, self->normal));
}

t_cylinder	*make_cylinder(t_cylinder cylinder)
{
	t_cylinder *c;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	c->base.hit = hit_cylinder;
	c->base.destroy = NULL;
	c->base.resize = resize_cylinder;
	c->base.rotate = rotate_cylinder;
	return (c);
}