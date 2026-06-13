/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 12:19:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/12 12:19:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"
#include "quaternion.h"
#include <stdlib.h>

static void	pad_to_minimums(t_aabb *p)
{
	float	delta;

	delta = 0.0001;
	if (p->x.size < delta)
		p->x = p->x.expand(&(p->x), delta);
	if (p->y.size < delta)
		p->y = p->y.expand(&(p->y), delta);
	if (p->z.size < delta)
		p->z = p->z.expand(&(p->z), delta);
}

t_aabb	make_aabb_from_aabbs(t_aabb box0, t_aabb box1)
{
	t_aabb	merged;

	merged.x = interval_init(fmin(box0.x.min, box1.x.min), fmax(box0.x.max,
				box1.x.max));
	merged.y = interval_init(fmin(box0.y.min, box1.y.min), fmax(box0.y.max,
				box1.y.max));
	merged.z = interval_init(fmin(box0.z.min, box1.z.min), fmax(box0.z.max,
				box1.z.max));
	return (merged);
}

t_aabb	make_aabb(t_vec3 a, t_vec3 b)
{
	t_aabb	ab;

	ab.x = interval_init(fmin(a.x, b.x), fmax(a.x, b.x));
	ab.y = interval_init(fmin(a.y, b.y), fmax(a.y, b.y));
	ab.z = interval_init(fmin(a.z, b.z), fmax(a.z, b.z));
	pad_to_minimums(&ab);
	return (ab);
}

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

//TODO: too many vars
bool	hit_quad(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_quad	*self;
	float	denom;
	float	t;
	t_vec3	intersection;
	t_vec3	p;
	float	alpha;
	float	beta;

	self = (t_quad *)base;
	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (dot(self->normal, self->q) - dot(self->normal, ray.origin)) / denom;
	if (!ray_t.contains(&ray_t, t))
		return (false);
	intersection = ray.at(&ray, t);
	p = sub(intersection, self->q);
	alpha = dot(self->w, cross(p, self->v));
	beta = dot(self->w, cross(self->u, p));
	if (!is_interior(alpha, beta, rec))
		return (false);
	rec->t = t;
	rec->p = intersection;
	rec->mat = self->mat;
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
	t_aabb	bbox_diagonal1;
	t_aabb	bbox_diagonal2;
	t_vec3	n;

	quad = malloc(sizeof(t_quad));
	if (!quad)
		return (NULL);
	quad->q = q;
	quad->u = u;
	quad->v = v;
	quad->mat = mat;
	bbox_diagonal1 = make_aabb(q, add(q, add(u, v)));
	bbox_diagonal2 = make_aabb(add(q, u), add(q, v));
	quad->bbox = make_aabb_from_aabbs(bbox_diagonal1, bbox_diagonal2);
	quad->base.hit = hit_quad;
	//quad->base.destroy = NULL;
	quad->base.resize = NULL;
	quad->base.rotate = rotate_quad;
	n = cross(u, v);
	quad->normal = norm(n);
	quad->d = dot(quad->normal, q);
	quad->w = divide(n, dot(n, n));
	pad_to_minimums(&quad->bbox);
	return (quad);
}
