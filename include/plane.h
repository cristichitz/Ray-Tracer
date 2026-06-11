/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:28 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 19:36:42 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLANE_H
# define PLANE_H

# include "hittable.h"
# include "types.h"

typedef struct s_aabb {
	t_interval	x;
	t_interval	y;
	t_interval	z;
} t_aabb;

typedef struct s_quad
{
	t_hittable	base;
	t_vec3		Q;
	t_vec3		u;
	t_vec3		v;
	t_material	mat;
	t_aabb		bbox;
	t_vec3		normal;
	float		D;
	t_vec3		w;
}	t_quad;

typedef struct s_plane
{
	t_hittable	base;
	t_vec3		Q;
	t_vec3		normal;
	t_material	mat;
	float		D;

}	t_plane;

typedef	struct s_box {
	t_hittable	base;
	t_quad		*sides[6];
} t_box;

// bool			hit_plane(void *base, t_ray ray, t_interval t, t_hit_record *rec);

t_aabb			make_aabb(t_vec3 a, t_vec3 b);			
t_quad			*make_quad(t_vec3 Q, t_vec3 u, t_vec3 v, t_material mat);
t_plane			*make_infinite_plane(t_vec3 center, t_vec3 normal, t_vec3 colour);
t_box			*make_box(t_vec3 a, t_vec3 b, t_material mat);

#endif