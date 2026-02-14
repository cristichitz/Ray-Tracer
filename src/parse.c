/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/02/14 17:30:03 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

static int	valid_filename(char *filename, const char *ext)
{
	const char		*dot;
	unsigned int	ext_len;
	unsigned int	dot_len;

	dot = ft_strrchr(filename, '.');
	if (!dot || dot == filename)
	{
		print_error("Please input valid .rt files.");
		return (EXIT_FAILURE);
	}
	ext_len = ft_strlen(ext);
	dot_len = ft_strlen(dot);
	if (!(ft_strncmp(dot, ext, ext_len) == 0) || !(ft_strncmp(ext, dot,
				dot_len) == 0))
	{
		print_error("Incorrect file type, only .rt accepted.");
		return (EXIT_FAILURE);
	}
	else
		return (!(dot[ext_len] == '\0'));
}

int	parse_input(int ac, char **av)
{
	int		fd;
	char	*file;

	if (ac != 2)
	{
		print_error("Please provide one argument.");
	}
	if (valid_filename(av[1], ".rt"))
	{
		fd = open(av[1], O_RDONLY);
	}
}

// Check for valid filename
// Check if file exists
// 
//