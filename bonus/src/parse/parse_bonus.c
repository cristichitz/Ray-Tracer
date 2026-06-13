/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:22 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:22 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"
#include <fcntl.h>

static int	valid_ext(char *file)
{
	char	*dot;

	dot = ft_strrchr(file, '.');
	if (!dot || ft_strcmp(dot, ".rt"))
		return (parse_err("Scene file must have a .rt extension."));
	return (1);
}

static int	process_type(t_data *data, char **p)
{
	if (!ft_strcmp(p[0], "C"))
		return (set_cam(data, p));
	if (!ft_strcmp(p[0], "A"))
		return (set_ambient(data, p));
	if (!ft_strcmp(p[0], "L"))
		return (set_light(data, p));
	if (!ft_strcmp(p[0], "sp"))
		return (set_sphere(data, p));
	if (!ft_strcmp(p[0], "pl"))
		return (set_plane(data, p));
	if (!ft_strcmp(p[0], "cy"))
		return (set_cylinder(data, p));
	return (parse_err("Unknown element type."));
}

static int	process_line(t_data *data, char *line)
{
	char	**p;
	int		res;
	int		i;

	i = 0;
	while (line[i])
	{
		if (line[i] == '\t')
			line[i] = ' ';
		i++;
	}
	p = ft_split(line, ' ');
	if (!p)
		return (0);
	if (!p[0])
		return (ft_free_split(p), 1);
	res = process_type(data, p);
	ft_free_split(p);
	return (res);
}

int	parse_scene(t_data *data, char *file)
{
	int		fd;
	char	*line;
	char	*trimmed;

	if (!valid_ext(file))
		return (0);
	fd = open(file, O_RDONLY);
	if (fd < 0)
		return (parse_err("Could not open scene file."));
	line = get_next_line(fd);
	while (line)
	{
		trimmed = ft_strtrim(line, " \t\n");
		free(line);
		if (!trimmed || !process_line(data, trimmed))
			return (free(trimmed), close(fd), 0);
		free(trimmed);
		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}

int	load_scene(t_data *data, int argc, char **argv)
{
	int	i;

	data->cam_center = make_float3(0.0f, 0.0f, 0.0f);
	data->cam_dir = make_float3(0.0f, 0.0f, -1.0f);
	data->cam_fov = 70.0f;
	data->frame.background = make_float3(0.70f, 0.80f, 1.0f);
	i = 1;
	while (i < argc)
	{
		if (!ft_strcmp(argv[i], "--wall"))
			return (build_wall_scene(data), 1);
		i++;
	}
	while (--argc > 0)
	{
		if (argv[argc][0] != '-')
			return (parse_scene(data, argv[argc]));
	}
	// build_rubik(data);
	make_cornell_box(data);
	return (1);
}
