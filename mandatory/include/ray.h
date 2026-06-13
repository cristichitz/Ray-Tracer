/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:35:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 17:35:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "vec3.h"

typedef struct s_ray		t_ray;

typedef t_vec3				(*t_ray_at)(t_ray *self, float t);

typedef struct s_ray
{
	t_vec3					origin;
	t_vec3					dir;
	t_ray_at				at;
}							t_ray;

t_ray						make_ray(t_vec3 origin, t_vec3 direction);

#endif
