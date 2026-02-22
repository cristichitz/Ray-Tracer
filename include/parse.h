/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:18:29 by timurray          #+#    #+#             */
/*   Updated: 2026/02/22 19:19:22 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "libft.h"
# include "rt_cpu.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>

#define COORD_MAX 2000.0 //TODO: What should max be?
#define COORD_MIN -2000.0 //TODO: What should max be?
typedef struct s_range
{
	float	max;
	float	min;
}			t_range;

typedef struct s_amb_light
{
	float brightness; // [0.0,1.0]
						// rgb
}			t_amb_light;

typedef struct s_light
{
	// coord
	float brightness; // [0.0,1.0]
						// rgb
}			t_light;

// typedef struct s_sphere
// {
// 	// coord
// 	float	d;
// 	// rgb
// }			t_sphere;

typedef struct s_plane
{
	// coord
	// 3D normalised vector
	// rgb
}			t_plane;

typedef struct s_cylinder
{
	// coord
	// 3d normalized
	float	d;
	float	h;
	// rgb
}			t_cylinder;

void		print_error(char *msg);
int			parse_input(t_data *data, int ac, char **av);

#endif