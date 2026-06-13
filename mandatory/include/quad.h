/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 12:23:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/12 12:23:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUAD_H
# define QUAD_H

# include "hittable.h"

typedef struct s_aabb
{
	t_interval	x;
	t_interval	y;
	t_interval	z;
}				t_aabb;

typedef struct s_quad
{
	t_hittable	base;
	t_vec3		q;
	t_vec3		u;
	t_vec3		v;
	t_material	mat;
	t_aabb		bbox;
	t_vec3		normal;
	float		d;
	t_vec3		w;
}				t_quad;

t_aabb			make_aabb(t_vec3 a, t_vec3 b);
t_quad			*make_quad(t_vec3 q, t_vec3 u, t_vec3 v, t_material mat);

#endif
