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


bool	hit_box(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_box	*self;
	t_hit_record	tmp;
	bool			hit;
	float			closest;
	int				i;

	self = (t_box *)base;
	tmp.set_face_normal = ft_set_face_normal;
	hit = false;
	closest = t.max;
	i = 0;
	while (i < 6)
	{
		if (self->sides[i]->base.hit(self->sides[i], ray, interval_init(t.min, closest), &tmp))
		{
			hit = true;
			closest = tmp.t;
			*rec = tmp;
		}
		i++;
	}
	return (hit);
}

t_box	*make_box(t_vec3 a, t_vec3 b, t_material mat)
{
	t_box *boxi;
	t_vec3 min;
	t_vec3 max;

	t_vec3 dx;
	t_vec3 dy;
	t_vec3 dz;

	boxi = malloc(sizeof(t_box));
	if (!boxi)
		return (NULL);

	min = make_vec(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
	max = make_vec(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

	dx = make_vec(max.x - min.x, 0.0f, 0.0f);
	dy = make_vec(0.0f, max.y - min.y, 0.0f);
	dz = make_vec(0.0f, 0.0f, max.z - min.z);

	boxi->sides[0] = make_quad(make_vec(min.x, min.y, max.z), dx, dy, mat); // front
	boxi->sides[1] = make_quad(make_vec(max.x, min.y, max.z), scale(dz, -1), dy, mat); // right
	boxi->sides[2] = make_quad(make_vec(max.x, min.y, min.z), scale(dx, -1), dy, mat); // back

	boxi->sides[3] = make_quad(make_vec(min.x, min.y, min.z), dz, dy, mat); // left
	boxi->sides[4] = make_quad(make_vec(min.x, max.y, max.z), dx, scale(dz, -1), mat); // top
	boxi->sides[5] = make_quad(make_vec(min.x, min.y, min.z), dx, dz, mat); // bottom

	boxi->base.hit = hit_box;
	return (boxi);
}