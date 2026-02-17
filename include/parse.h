/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:18:29 by timurray          #+#    #+#             */
/*   Updated: 2026/02/15 13:39:57 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "libft.h"
# include <fcntl.h>
# include <stdlib.h>

typedef struct s_amb_light
{
	float brightness; // [0.0,1.0]
						// rgb
}			t_amb_light;

typedef struct s_cam
{
	// coord
	// 3d Normalised vector
	// fov
}			t_cam; 

typedef struct s_light
{
	// coord
	float brightness; // [0.0,1.0]
						// rgb
}			t_light;

typedef struct s_sphere
{
	// coord
	float	d;
	// rgb
}			t_sphere;

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
int			parse_input(int ac, char **av);

#endif