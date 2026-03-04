/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 13:30:57 by timurray          #+#    #+#             */
/*   Updated: 2026/03/04 14:48:32 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

typedef struct s_rgb
{
	int		r;
	int		g;
	int		b;
}			t_rgb;

typedef struct s_pt
{
	float	x;
	float	y;
	float	z;
}			t_pt;

typedef struct s_vec3
{
	float	x;
	float	y;
	float	z;
}			t_vec3;

#endif