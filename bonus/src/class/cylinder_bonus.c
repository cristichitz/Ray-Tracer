/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:53 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:54 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Finite, capped cylinder. `center` is the midpoint of the axis and `axis` is
** the unit direction (the parser normalises it). The kernel
** (bonus/kernels/hit_cylinder_bonus.c) works from the midpoint + half-height,
** so d_top is kept only for reference. Radius and height travel in a small
** struct to stay within the 4-parameter limit.
*/
t_object	make_obj_cylinder(cl_float3 center, cl_float3 axis,
				t_cyl_size size, t_material mat)
{
	t_object	o;

	memset(&o, 0, sizeof(o));
	o.type = OBJ_CYLINDER;
	o.center = center;
	o.normal = axis;
	o.radius = size.radius;
	o.height = size.height;
	o.d_top = dot(axis, center) + size.height / 2.0f;
	o.material = mat;
	return (o);
}
