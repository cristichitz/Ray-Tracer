/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:34:42 by timurray          #+#    #+#             */
/*   Updated: 2026/03/06 11:26:43 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIGHT_H
# define LIGHT_H

# include "types.h"

typedef struct s_ambient_light
{
	float	brightness;
	t_vec3	colour;
}			t_ambient_light;

typedef struct s_light
{
	t_vec3	center;
	float	brightness;
	t_vec3	colour;
}			t_light;

#endif