/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:06:28 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 13:40:34 by timurray         ###   ########.fr       */
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
	t_vec3		uvec;
	t_rgb		colour;
}				t_plane;

t_plane			*make_plane(t_plane plane);

#endif