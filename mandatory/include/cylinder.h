/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:33 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 13:40:17 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CYLINDER_H
# define CYLINDER_H

# include "hittable.h"

typedef struct s_cylinder
{
	t_hittable	base;
	t_vec3		center;
	t_vec3		normal;
	float		radius;
	float		height;
	t_material	mat;
}				t_cylinder;

typedef struct s_cyl_var
{
	t_vec3		w;
	float		a;
	float		half_b;
	float		c;
	float		dv;
	float		wv;
	float		discriminant;
	float		sqrtd;
	float		root;
	float		m;
}				t_cyl_var;

typedef struct s_cap
{
	t_vec3		center;
	t_vec3		normal;
	float		radius;
	t_material	mat;
}				t_cap;

bool			hit_cyl_body(t_cylinder *self, t_ray ray, t_interval t,
					t_hit_record *rec);
bool			hit_caps(void *base, t_ray ray, t_interval ray_t,
					t_hit_record *rec);
t_cylinder		*make_cylinder(t_cylinder cylinder);

#endif