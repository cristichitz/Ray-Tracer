/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cyclinder.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:33 by timurray          #+#    #+#             */
/*   Updated: 2026/03/04 15:06:43 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CYLINDER_H
# define CYLINDER_H

# include "hittable.h"
# include "types.h"

typedef struct s_cylinder
{
	t_hittable	base;
	t_vec3		cylinder;
	float		radius;
	float		height;
	t_rgb		colour;
}				t_cylinder;

// bool			hit_cylinder(void *base, t_ray ray, t_interval t, t_hit_record *rec);
// t_cylinder		*make_cylinder(t_vec3 center, float radius, float height);

#endif