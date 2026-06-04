/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:33 by timurray          #+#    #+#             */
/*   Updated: 2026/06/02 17:33:02 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CYLINDER_H
# define CYLINDER_H

# include "hittable.h"
# include "types.h"

typedef struct s_cylinder
{
	t_hittable	base;
	t_vec3		center;		
	t_vec3		uvec;
	float		radius;
	float		height;
	t_material	mat;
}				t_cylinder;

bool			hit_cylinder(void *base, t_ray ray, t_interval t,
					t_hit_record *rec);
t_cylinder		*make_cylinder(t_cylinder cylinder);

#endif