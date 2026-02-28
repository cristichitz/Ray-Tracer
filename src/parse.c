/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/02/28 19:03:42 by timurray         ###   ########.fr       */
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

int	convert_float_limit(float *num, char *param, float min, float max)
{
	float	fnum;

	fnum = ft_strtof(param, NULL);
	if (fnum > max)
	{
		print_error("val too large.");
		return (0);
	}
	if (fnum < min)
	{
		print_error("val too low");
		return (0);
	}
	*num = fnum;
	return (1);
}

int	convert_int_limit(int *num, char *param, int min, int max)
{
	int	n;

	n = ft_atoi(param);
	if (n > max)
	{
		print_error("val too large.");
		return (0);
	}
	if (n < min)
	{
		print_error("val too low");
		return (0);
	}
	*num = n;
	return (1);
}

int	get_pt(float *num, char *param)
{
	if (!convert_float_limit(num, param, COORD_MIN, COORD_MAX))
		return (0);
	return (1);
}

int	get_udir(float *num, char *param)
{
	if (!convert_float_limit(num, param, 0.0, 1.0))
		return (0);
	return (1);
}

int	get_fov(int *num, char *param)
{
	if (!convert_int_limit(num, param, 0, 180))
		return (0);
	return (1);
}

int	set_pts(t_pt *pt, char **params, int index, int (*f)(float *n, char *p))
{
	char		**str_pts;
	const char	delim = ',';
	int			err;

	err = 0;
	str_pts = ft_split(params[index], delim);
	//TODO: ftsplit check
	if (!f(&pt->x, str_pts[0]))
		err++;
	if (!f(&pt->y, str_pts[1]))
		err++;
	if (!f(&pt->z, str_pts[2]))
		err++;
	ft_free_split(str_pts);
	if (err > 0)
		return (0);
	return (1);
}

int	set_cam(t_data *data, char **params)
{
	char	**str_pts;

	if (data->set_cam == true)
	{
		print_error("Duplicate camera entry.");
		return (0);
	}
	if (!set_pts(&data->cam.pt, params, 1, get_pt))
		return (0);
	if (!set_pts(&data->cam.udir, params, 2, get_udir))
		return (0);
	if (!get_fov(&data->cam.fov, params[3]))
		return (0);
	data->set_cam = true;
	return (1);
}
int	set_ambient_light(t_data *data, char **params)
{
	char	**str_pts;

	if (data->set_ambient_light == true)
	{
		print_error("Duplicate ambient light entry.");
		return (0);
	}
	data->ambient_light.brightness = ft_atof(params[1]);
	str_pts = ft_split(params[2], ',');
	// data->ambient_light.colour.r = ft_atoi();
	data->set_ambient_light = true;
	return (1);
}

int	set_light(t_data *data, char **params)
{
	if (data->set_light == true)
	{
		print_error("Duplicate light entry.");
		return (0);
	}
	return (1);
}

int	set_sphere(void)
{
	return (1);
}

int	set_cylinder(void)
{
	return (1);
}

int	set_plane(void)
{
	return (1);
}

int	process_line(t_data *data, char *line)
{
	char	**params;

	params = ft_split(line, ' ');
	if (params[0][0] != '\n')
	{
		if (ft_strncmp(params[0], "C", 1) == 0)
		{
			set_cam(data, params);
		}
		else if (ft_strcmp(params[0], "A") == 0)
		{
			set_ambient_light(data, params);
		}
		else if (ft_strcmp(params[0], "L") == 0)
		{
			set_light();
		}
		else if (ft_strcmp(params[0], "pl") == 0)
		{
			set_plane();
		}
		else if (ft_strcmp(params[0], "sp") == 0)
		{
			set_sphere();
		}
		else if (ft_strcmp(params[0], "cy") == 0)
		{
			set_cylinder();
		}
		else
		{
			print_error("type not found");
			return (0);
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
	if (!line)
	{
		print_error("Empty file.");
		return (0);
	}
	// TODO: if no first  line?
	while (line)
	{
		process_line(data, ft_strtrim(line, " "));
		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}

/*


TODO: Convert float values
TODO: convert unsigned int values
TODO: handle error flow

TODO: create cam
TODO: create ambient light
TODO: create sphere
TODO: create cylinder
TODO: create plane
TODO: create light

TODO: Apply udir where applicable.

*/