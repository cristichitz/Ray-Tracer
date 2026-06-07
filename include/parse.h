/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:18:29 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 18:24:15 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "libft.h"
# include "rt_cpu.h"
# include <fcntl.h>
# include <float.h>
# include <limits.h>
# include <stdio.h>
# include <stdlib.h>

# define COORD_MAX 2000.0  // TODO: What should max be?
# define COORD_MIN -2000.0 // TODO: What should min be?
# define LIGHT_QUAD_SIZE 20.0f // side length of the emissive quad spawned for L
# define LIGHT_QUAD_GAIN 4.0f  // emission multiplier so the quad lights the scene

void	print_error(char *msg);
int		parse_input(t_data *data, int ac, char **av);
int		return_print_error(char *msg, int err);

#endif