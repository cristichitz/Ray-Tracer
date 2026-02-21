/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/02/21 20:56:32 by timurray         ###   ########.fr       */
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
		return (0);
	}
	ext_len = ft_strlen(ext);
	dot_len = ft_strlen(dot);
	if (!(ft_strncmp(dot, ext, ext_len) == 0) || !(ft_strncmp(ext, dot,
				dot_len) == 0))
	{
		print_error("Incorrect file type, only .rt accepted.");
		return (0);
	}
	else
		return ((dot[ext_len] == '\0'));
}

int set_cam(char *param)
{
	
}

int	process_line(char *data)
{
	char **components;

	components = ft_split(data, ' ');
	if(components[0][0] != '\n')
		printf("Object type: %s\n", components[0]);
	return (1);
}

int	parse_input(int ac, char **av)
{
	int		fd;
	char	*line;

	if (ac != 2)
	{
		print_error("Please provide one argument.");
		return (0);
	}
	if (!valid_filename(av[1], ".rt"))
		return (0);
	fd = open(av[1], O_RDONLY);
	if (fd == -1)
	{
		print_error("File not found.");
		return (0);
	}
	line = get_next_line(fd);
	while (line)
	{
		// printf("line: %s\n", line);
		process_line(ft_strtrim(line, " "));

		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}

// Check for valid filename
// Check if file exists
// Read each line
// vector each pointer