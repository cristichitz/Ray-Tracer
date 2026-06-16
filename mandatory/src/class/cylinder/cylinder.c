/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 19:52:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "quaternion.h"
#include <stdlib.h>

bool	hit_cylinder(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_cylinder	*self;
	bool		hit_body;

	self = (t_cylinder *)base;
	hit_body = hit_cyl_body(self, ray, t, rec);
	if (hit_body)
		t.max = rec->t;
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

static void	material_cylinder(void *base)
{
	t_cylinder	*self;

	self = (t_cylinder *)base;
	if (self->mat.type == MAT_LAMBERTIAN)
		self->mat = init_metal(self->mat.tex.albedo);
	else if (self->mat.type == MAT_METAL)
		self->mat = init_lambertian(self->mat.tex.albedo);
}

t_cylinder	*make_cylinder(t_cylinder cylinder)
{
	t_cylinder	*c;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	c->base.hit = hit_cylinder;
	c->base.resize = resize_cylinder;
	c->base.rotate = rotate_cylinder;
	c->base.destroy = NULL;
	c->base.material = material_cylinder;
	return (c);
}
