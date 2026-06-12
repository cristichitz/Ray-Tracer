/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 17:05:37 by timurray          #+#    #+#             */
/*   Updated: 2026/06/12 12:23:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "plane.h"
#include "quaternion.h"
#include <stdlib.h>

bool	hit_plane(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_plane	*self;
	float	denom;
	float	t;
	t_vec3	intersection;

	self = (t_plane *)base;
	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);
	t = (dot(self->normal, self->Q) - dot(self->normal, ray.origin)) / denom;
	if (!ray_t.contains(&ray_t, t))
		return (false);
	intersection = ray.at(&ray, t);
	rec->p = intersection;
	rec->t = t;
	rec->mat = self->mat;
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

void	rotate_plane(void *base, t_vec3 axis, float angle)
{
	t_plane			*self;
	t_quaternion	q;

	self = (t_plane *)base;
	q = make_quaternion(axis, angle);
	self->normal = norm(rotate_vec_by_quaternion(q, self->normal));
}

t_plane	*make_plane(t_plane plane)
{
	t_plane	*p;

	p = malloc(sizeof(t_plane));
	if (!p)
		return (NULL);
	*p = plane;
	p->D = dot(p->normal, p->Q);
	p->base.hit = hit_plane;
	//p->base.destroy = NULL;
	p->base.resize = NULL;
	p->base.rotate = rotate_plane;
	return (p);
}
