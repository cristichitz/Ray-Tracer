/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:06:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:44:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "interval.h"
# include "material.h"
# include "ray.h"

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
	void					(*set_face_normal)(struct s_hit_record *self,
							t_ray ray, t_vec3 outward_normal);
}							t_hit_record;

typedef struct s_hittable
{
	bool					(*hit)(void *object, t_ray ray, t_interval t,
							t_hit_record *rec);
	void					(*destroy)(void *object);
	void					(*resize)(void *object, float scalar);
	void					(*rotate)(void *object, t_vec3 axis, float angle);
}							t_hittable;

void						ft_set_face_normal(t_hit_record *self, t_ray r,
								t_vec3 outward_normal);
#endif
