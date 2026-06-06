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

t_cylinder		*make_cylinder(t_cylinder cylinder);

#endif