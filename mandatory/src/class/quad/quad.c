/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 12:19:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 20:55:55 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"
#include "quaternion.h"

bool	is_interior(float a, float b, t_hit_record *rec)
{
	t_interval	ui;

	ui = interval_init(0, 1);
	if (!ui.contains(&ui, a) || !ui.contains(&ui, b))
		return (false);
	rec->u = a;
	rec->v = b;
	return (true);
}

bool	quad_intersection(t_quad *self, t_vec3 intersection, t_hit_record *rec)
{
	t_vec3	p;
	float	alpha;
	float	beta;

	p = sub(intersection, self->q);
	alpha = dot(self->w, cross(p, self->v));
	beta = dot(self->w, cross(self->u, p));
	if (!is_interior(alpha, beta, rec))
		return (false);
	rec->p = intersection;
	rec->mat = self->mat;
	return (true);
}

bool	hit_quad(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_quad	*self;
	float	denom;
	float	t;
	t_vec3	intersection;

	self = (t_quad *)base;
	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (dot(self->normal, self->q) - dot(self->normal, ray.origin)) / denom;
	if (!ray_t.contains(&ray_t, t))
		return (false);
	intersection = ray.at(&ray, t);
	if (!quad_intersection(self, intersection, rec))
		return (false);
	rec->t = t;
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

void	rotate_quad(void *base, t_vec3 axis, float angle)
{
	t_quad			*self;
	t_quaternion	q;
	t_vec3			n;

	self = (t_quad *)base;
	q = make_quaternion(axis, angle);
	self->u = rotate_vec_by_quaternion(q, self->u);
	self->v = rotate_vec_by_quaternion(q, self->v);
	n = cross(self->u, self->v);
	self->normal = norm(n);
	self->d = dot(self->normal, self->q);
	self->w = divide(n, dot(n, n));
}

t_quad	*make_quad(t_vec3 q, t_vec3 u, t_vec3 v, t_material mat)
{
	t_quad	*quad;
	t_vec3	n;

	quad = malloc(sizeof(t_quad));
	if (!quad)
		return (NULL);
	quad->q = q;
	quad->u = u;
	quad->v = v;
	quad->mat = mat;
	quad->bbox = make_aabb_from_aabbs(make_aabb(q, add(q, add(u, v))),
			make_aabb(add(q, u), add(q, v)));
	quad->base.hit = hit_quad;
	quad->base.resize = NULL;
	quad->base.rotate = rotate_quad;
	quad->base.destroy = NULL;
	quad->base.material = NULL;
	n = cross(u, v);
	quad->normal = norm(n);
	quad->d = dot(quad->normal, q);
	quad->w = divide(n, dot(n, n));
	pad_to_minimums(&quad->bbox);
	return (quad);
}
