/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:13:31 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	parse_input(t_data *data, int ac, char **av)
{
	int	fd;
	int	ok;

	if (ac != 2)
		return (return_print_error("Please provide one argument.", 0));
	if (!valid_filename(av[1], ".rt"))
		return (0);
	fd = open(av[1], O_RDONLY);
	if (fd == -1)
		return (return_print_error("File not found.", 0));
	ok = process_scene(data, fd);
	close(fd);
	if (!ok)
		return (0);
	return (valid_scene(data));
}
