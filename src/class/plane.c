/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 17:05:37 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 19:37:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "plane.h"

static void pad_to_minimums(t_aabb *p)
{
	float delta = 0.0001;

	if (p->x.size < delta)
		p->x = p->x.expand(&(p->x), delta);
	if (p->y.size < delta)
		p->y = p->y.expand(&(p->y), delta);
	if (p->z.size < delta)
		p->z = p->z.expand(&(p->z), delta);
}

t_aabb make_aabb_from_aabbs(t_aabb box0, t_aabb box1)
{
	t_aabb merged;

	merged.x = interval_init(fmin(box0.x.min, box1.x.min), fmax(box0.x.max, box1.x.max));
	merged.y = interval_init(fmin(box0.y.min, box1.y.min), fmax(box0.y.max, box1.y.max));
	merged.z = interval_init(fmin(box0.z.min, box1.z.min), fmax(box0.z.max, box1.z.max));

	return merged;
}

t_aabb make_aabb(t_vec3 a, t_vec3 b)
{
	t_aabb ab;

	ab.x = interval_init(fmin(a.x, b.x), fmax(a.x, b.x));
	ab.y = interval_init(fmin(a.y, b.y), fmax(a.y, b.y));
	ab.z = interval_init(fmin(a.z, b.z), fmax(a.z, b.z));

	pad_to_minimums(&ab);

	return (ab);
}

bool	is_interior(float a, float b, t_hit_record *rec)
{
	// Unit Interval
	t_interval ui;

	ui = interval_init(0, 1);

	if (!ui.contains(&ui, a) || !ui.contains(&ui, b))
		return (false);

	rec->u = a;
	rec->v = b;
	return (true);
}

bool hit_quad(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_quad *self;
	float denom;
	float t;
	t_vec3 intersection;
	t_vec3 p;

	self = (t_quad *)base;

	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);

	t = (self->D - dot(self->normal, ray.origin)) / denom;

	if (!ray_t.contains(&ray_t, t))
		return (false);

	intersection = ray.at(&ray, t);

	p = sub(intersection, self->Q);

	float alpha = dot(self->w, cross(p, self->v));
	float beta = dot(self->w, cross(self->u, p));

	if (!is_interior(alpha, beta, rec))
		return (false);
	rec->t = t;
	rec->p = intersection;
	rec->mat = self->mat;
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

t_quad *make_quad(t_vec3 Q, t_vec3 u, t_vec3 v, t_material mat)
{
	t_quad *quad;
	t_aabb bbox_diagonal1;
	t_aabb bbox_diagonal2;
	t_vec3 n;

	quad = malloc(sizeof(t_quad));
	if (!quad)
		return (NULL);

	quad->Q = Q;
	quad->u = u;
	quad->v = v;
	quad->mat = mat;
	bbox_diagonal1 = make_aabb(Q, add(Q, add(u, v)));
	bbox_diagonal2 = make_aabb(add(Q, u), add(Q, v));
	quad->bbox = make_aabb_from_aabbs(bbox_diagonal1, bbox_diagonal2);
	quad->base.hit = hit_quad;

	n = cross(u, v);
	quad->normal = norm(n);
	quad->D = dot(quad->normal, Q);
	quad->w = divide(n, dot(n, n));

	pad_to_minimums(&quad->bbox);
	return (quad);
}

bool hit_plane(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	t_plane *self;
	float denom;
	float t;
	t_vec3 intersection;

	self = (t_plane *)base;

	denom = dot(self->normal, ray.dir);
	if (fabs(denom) < 1e-8)
		return (false);

	t = (self->D - dot(self->normal, ray.origin)) / denom;

	if (!ray_t.contains(&ray_t, t))
		return (false);
	
	intersection = ray.at(&ray, t);

	rec->p = intersection;
	rec->t = t;
	rec->mat = self->mat;
	rec->set_face_normal(rec, ray, self->normal);
	return (true);
}

t_plane *make_infinite_plane(t_vec3 center, t_vec3 normal, t_vec3 colour)
{
	t_plane *p;

	p = malloc(sizeof(t_plane));
	if (!p)
		return (NULL);

	p->Q = center;
	p->normal = normal;
	p->mat = init_lambertian(colour);
	p->base.hit = hit_quad;
	p->D = dot(p->normal, p->Q);
	p->base.hit = hit_plane;

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