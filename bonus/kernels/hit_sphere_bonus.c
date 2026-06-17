/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_sphere_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:18 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:51:21 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

static void	get_sphere_uv(const float3 p, float *u, float *v)
{
	float	theta;
	float	phi;

	theta = acos(-p.y);
	phi = atan2(-p.z, p.x) + M_PI;
	*u = phi / (2 * M_PI);
	*v = theta / M_PI;
}

static float	get_sphere_root(t_sphere_var *v, t_interval *t)
{
	v->discriminant = v->h * v->h - v->a * v->c;
	if (v->discriminant < 0)
		return (-1);
	v->sqrtd = sqrt(v->discriminant);
	v->root = (v->h - v->sqrtd) / v->a;
	if (!interval_surrounds(t, v->root))
	{
		v->root = (v->h + v->sqrtd) / v->a;
		if (!interval_surrounds(t, v->root))
			return (-1);
	}
	return (v->root);
}

bool	hit_sphere(t_object self, t_ray ray, t_interval t, t_hit_record *rec)
{
	float3			oc;
	float3			outward_normal;
	t_sphere_var	v;

	oc = self.center - ray.origin;
	v.a = dot(ray.dir, ray.dir);
	v.h = dot(ray.dir, oc);
	v.c = dot(oc, oc) - self.radius * self.radius;
	if (get_sphere_root(&v, &t) < 0)
		return (false);
	rec->t = v.root;
	rec->p = ray_at(&ray, rec->t);
	rec->mat = self.material;
	outward_normal = (rec->p - self.center) / self.radius;
	ft_set_face_normal(rec, ray, outward_normal);
	get_sphere_uv(outward_normal, &rec->u, &rec->v);
	return (true);
}
