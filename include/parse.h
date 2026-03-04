/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:18:29 by timurray          #+#    #+#             */
/*   Updated: 2026/03/04 14:45:58 by timurray         ###   ########.fr       */
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
# include <float.h>

#define COORD_MAX 2000.0 //TODO: What should max be?
#define COORD_MIN -2000.0 //TODO: What should max be?

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