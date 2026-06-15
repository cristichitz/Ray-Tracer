/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:49 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:49 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Infinite plane. We precompute D = dot(normal, Q) on the host so the kernel
** (bonus/kernels/hit_plane_bonus.c) only does a ray-plane test. `normal` is
** assumed already unit length (the parser normalises it).
*/
t_object	make_obj_plane(cl_float3 point, cl_float3 normal, t_material mat)
{
	t_object	o;

	memset(&o, 0, sizeof(o));
	o.type = OBJ_PLANE;
	o.center = point;
	o.normal = normal;
	o.d = dot(normal, point);
	o.material = mat;
	return (o);
}

/*
** Bounded quad spanned by edge vectors u, v from corner Q. Precompute the
** plane normal, D and the w vector (n / dot(n,n)) used for the planar
** alpha/beta inside-test in bonus/kernels/hit_quad_bonus.c.
*/
t_object	make_obj_quad(cl_float3 q, cl_float3 u, cl_float3 v, t_material mat)
{
	t_object	o;
	cl_float3	n;

	memset(&o, 0, sizeof(o));
	n = cross(u, v);
	o.type = OBJ_QUAD;
	o.center = q;
	o.u = u;
	o.v = v;
	o.normal = norm(n);
	o.d = dot(o.normal, q);
	o.w = divide(n, dot(n, n));
	o.material = mat;
	return (o);
}

/*
** Oriented ellipse used as the portal surface. Unlike a quad, `center` is the
** true center and u, v are the (perpendicular) semi-axis vectors, so the
** kernel's unit-disk test (hit_ellipse_bonus.c) bounds it. n / d / w mirror the
** quad so the planar coordinate test is identical.
*/
t_object	make_obj_ellipse(cl_float3 center, cl_float3 u, cl_float3 v,
		t_material mat)
{
	t_object	o;
	cl_float3	n;

	memset(&o, 0, sizeof(o));
	n = cross(u, v);
	o.type = OBJ_ELLIPSE;
	o.center = center;
	o.u = u;
	o.v = v;
	o.normal = norm(n);
	o.d = dot(o.normal, center);
	o.w = divide(n, dot(n, n));
	o.material = mat;
	return (o);
}
