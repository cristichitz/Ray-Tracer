/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:28 by timurray          #+#    #+#             */
/*   Updated: 2026/03/04 15:26:01 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLANE_H
# define PLANE_H

# include "hittable.h"
# include "types.h"

typedef struct s_plane
{
	t_hittable	base;
	t_vec3		center;
	t_vec3		u_pt;
	t_rgb		colour;
}				t_plane;

// bool			hit_plane(void *base, t_ray ray, t_interval t, t_hit_record *rec);
// t_sphere		*make_plane(t_vec3 center, t_vec3 u_pt);

#endif