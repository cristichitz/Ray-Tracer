/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 13:00:40 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 13:00:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOX_H
# define BOX_H

# include "hittable.h"
# include "plane.h"
# include "quad.h"
# include <stdlib.h>

typedef struct s_box
{
	t_hittable	base;
	t_quad		*sides[6];
}				t_box;

t_box			*make_box(t_vec3 a, t_vec3 b, t_material mat);
bool			hit_box(void *base, t_ray ray, t_interval t,
					t_hit_record *rec);

#endif