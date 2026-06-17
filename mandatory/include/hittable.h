/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:06:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/14 12:45:40 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "interval.h"
# include "material.h"
# include "ray.h"
# include <stdlib.h>

typedef void				(*t_set_normal)(t_hit_record *self,
				t_ray ray, t_vec3 outward_normal);

typedef struct s_hit_record
{
	t_vec3					p;
	t_vec3					normal;
	t_material				mat;
	float					t;
	float					u;
	float					v;
	bool					front_face;
	t_vec3					colour;
	t_set_normal			set_face_normal;
}							t_hit_record;

typedef struct s_hittable
{
	bool					(*hit)(void *object, t_ray ray,
		t_interval t, t_hit_record * rec);
	void					(*resize)(void *object, float scalar);
	void					(*rotate)(void *object, t_vec3 axis, float angle);
	void					(*destroy)(void *self);
	void					(*material)(void *self);
}							t_hittable;

void						ft_set_face_normal(t_hit_record *self, t_ray r,
								t_vec3 outward_normal);

#endif
