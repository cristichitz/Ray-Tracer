/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/02/23 15:55:03 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"
#include "rt_cpu.h"

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

int	convert_limit(float *num, char *param, t_range range)
{
	float fnum;

	fnum = ft_strtof(param, NULL);
	if (fnum > range.max)
	{
		print_error("val too large.");
		return (0);
	}
	if (fnum < range.min)
	{
		print_error("val too low");
		return (0);
	}
	*num = fnum;
	return (1);
}

int get_pt(float *num, char *param)
{
	t_range range;

	range.min = COORD_MIN;
	range.max = COORD_MAX;
	
	if(!convert_limit(num, param, range))
		return (0);
	return (1);
}

int get_udir(float *num, char *param)
{
	t_range range;

	range.min = 0.0;
	range.max = 1.0;
	
	if(!convert_limit(num, param, range))
		return (0);
	return (1);
}

int get_fov(float *num, char *param)
{
	t_range range;

	range.max = 0;
	range.min = 180.0;
	
	if(!convert_limit(num, param, range))
		return (0);
	return (1);
}

int	set_cam(t_data *data, char **params)
{
	char	**str_pts;

	str_pts = ft_split(params[1], ',');
	data->cam.pt.x = ft_atof(str_pts[0]);
	data->cam.pt.y = ft_atof(str_pts[1]);
	data->cam.pt.z = ft_atof(str_pts[2]);
	ft_free_split(str_pts);

	str_pts = ft_split(params[2], ',');
	data->cam.udir.pt.x = ft_atof(str_pts[0]);
	data->cam.udir.pt.y = ft_atof(str_pts[0]);
	data->cam.udir.pt.z = ft_atof(str_pts[0]);
	ft_free_split(str_pts);
	// data->cam.fov = ft_atoi(params[3]);
	if (!get_fov(data->cam.fov, params[3]))
		print("err on fov.\n");
	return (1);
}

int	process_line(t_data *data, char *line)
{
	char	**params;

	params = ft_split(line, ' ');
	if (params[0][0] != '\n')
	{
		// printf("Object type: %s\n", params[0]);
		// printf("Object type: %s\n", params[1]);
		if (ft_strncmp(params[0], "C", 1) == 0)
		{
			// printf("cam found: %s\n", params[0]);
			set_cam(data, params);
		}
	}
	return (1);
}

int	parse_input(t_data *data, int ac, char **av)
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
		process_line(data, ft_strtrim(line, " "));
		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}
/*
TODO: load the whole file into string vector.
TODO: Check for duplicate Capital letters.
TODO: Convert float values
TODO: convert unsigned int values
TODO: handle error flow

TODO: create cam
TODO: create sphere
TODO: create cylinder
TODO: create plane
TODO: create ambient light
TODO: create light

TODO: Apply udir where applicable.

*/