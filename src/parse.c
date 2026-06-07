/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:32:37 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 20:38:58 by timurray         ###   ########.fr       */
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

static size_t	split_len(char **split)
{
	size_t	len;

	len = 0;
	if (!split)
		return (0);
	while (split[len])
		len++;
	return (len);
}

static int	split_count(char **params, size_t expected)
{
	if (split_len(params) != expected)
	{
		print_error("Invalid number of arguments.");
		return (0);
	}
	return (1);
}

int	set_pts(t_vec3 *pt, char *params, int (*f)(float *n, char *p))
{
	char	**parts;
	int		ok;

	if (!params)
		return(return_print_error("Missing point parameter.", 0));
	ok = 1;
	parts = ft_split(params, ',');
	if (!parts)
		return (0);
	if (split_len(parts) != 3)
		ok = 0;
	else if (!f(&pt->x, parts[0]) || !f(&pt->y, parts[1]) || !f(&pt->z,
			parts[2]))
		ok = 0;
	ft_free_split(parts);
	return (ok);
}

int	get_float(float *num, char *param, float min, float max)
{
	float	fnum;

	fnum = ft_strtof(param, NULL); // TODO: Use endptr for check.
	if (fnum > max)
		return (return_print_error("Value too large.", 0));
	if (fnum < min)
		return (return_print_error("Value too low.", 0));
	*num = fnum;
	return (1);
}

int	get_int(int *num, char *param, int min, int max)
{
	int	n;

	n = ft_atoi(param); // TODO: Replace with safer check.
	if (n > max)
		return (return_print_error("Value too large.", 0));
	if (n < min)
		return (return_print_error("Value too low.", 0));
	*num = n;
	return (1);
}

int	get_pt(float *num, char *param)
{
	if (!get_float(num, param, COORD_MIN, COORD_MAX))
		return (0);
	return (1);
}

int	get_uvec_pt(float *num, char *param)
{
	if (!get_float(num, param, -1.0, 1.0))
		return (0);
	return (1);
}

int	set_fov(int *num, char *param)
{
	if (!get_int(num, param, 0, 180))
		return (0);
	return (1);
}

int	get_normed_float(float *num, char *param, float min, float max)
{
	float	fnum;

	fnum = ft_strtof(param, NULL); // TODO: Use endptr for check.
	if (fnum > max)
		return (return_print_error("Value too large.", 0));
	if (fnum < min)
		return (return_print_error("Value too low.", 0));
	fnum = fnum / 255.0f;
	*num = fnum;
	return (1);
}

int	set_colour(t_vec3 *colour, char *params)
{
	char	**parts;
	int		ok;

	parts = ft_split(params, ',');
	if (!parts)
		return (0);
	ok = 1;
	if (split_len(parts) != 3)
		ok = 0;
	else if (!get_normed_float(&colour->x, parts[0], 0, 255.0f)
		|| !get_normed_float(&colour->y, parts[1], 0, 255.0f)
		|| !get_normed_float(&colour->z, parts[2], 0, 255.0f))
		ok = 0;
	ft_free_split(parts);
	return (ok);
}

int	set_brightness(float *fnum, char *param)
{
	if (!get_float(fnum, param, 0.0, 1.0))
		return (0);
	return (1);
}

int	set_radius(float *num, char *param)
{
	float	diameter;

	if (!get_float(&diameter, param, 0.0, FLT_MAX))
		return (0);
	*num = diameter / 2.0f;
	return (1);
}

int	set_height(float *num, char *param)
{
	if (!get_float(num, param, 0.0, FLT_MAX))
		return (0);
	return (1);
}

int	set_cam(t_data *data, char **params)
{
	if (!split_count(params, 4))
		return (0);
	if (data->set_cam == true)
	{
		print_error("Duplicate camera entry.");
		return (0);
	}
	if (!set_pts(&data->cam.center, params[1], get_pt))
		return (0);
	if (!set_pts(&data->cam.uvec, params[2], get_uvec_pt))
		return (0);
	if (!set_fov(&data->cam.fov, params[3]))
		return (0);
	data->set_cam = true;
	return (1);
}

int	set_ambient_light(t_data *data, char **params)
{
	t_vec3 ambient;

	if (!split_count(params, 3))
		return (0);
	if (data->set_ambient_light == true)
	{
		print_error("Duplicate ambient light entry.");
		return (0);
	}
	if (!set_brightness(&data->ambient_light.brightness, params[1]))
		return (0);
	if (!set_colour(&data->ambient_light.colour, params[2]))
		return (0);
	ambient = scale(data->ambient_light.colour, data->ambient_light.brightness);
	data->world.ambient = ambient;
	// data->world.background = ambient;
	data->set_ambient_light = true;
	return (1);
}

int	add_light_quad(t_data *data, t_light light)
{
	t_vec3		u;
	t_vec3		v;
	t_vec3		corner;
	t_material	difflight;
	t_quad		*object;

	u = make_vec(LIGHT_QUAD_SIZE, 0.0f, 0.0f);
	v = make_vec(0.0f, 0.0f, LIGHT_QUAD_SIZE);
	corner = sub(light.center, scale(add(u, v), 0.5f));
	difflight = init_diffuse_light(scale(light.colour,
				light.brightness * LIGHT_QUAD_GAIN));
	object = make_quad(corner, u, v, difflight);
	if (!object)
		return (return_print_error("Failed to allocate light quad.", 0));
	if (data->world.add(&data->world, object) != EXIT_SUCCESS)
	{
		free(object);
		return (return_print_error("Failed to add light quad to world.", 0));
	}
	return (1);
}

int	set_light(t_data *data, char **params)
{
	if (!split_count(params, 4))
		return (0);
	if (data->set_light == true)
	{
		print_error("Duplicate light entry.");
		return (0);
	}
	if (!set_pts(&data->light.center, params[1], get_pt))
		return (0);
	if (!set_brightness(&data->light.brightness, params[2]))
		return (0);
	if (!set_colour(&data->light.colour, params[3]))
		return (0);
	if (!add_light_quad(data, data->light))
		return (0);
	data->set_light = true;
	return (1);
}

int	set_material(t_material *mat, char *params)
{
	char		**parts;
	int			ok;
	t_vec3		colour;

	colour = make_vec(0.0f, 0.0f, 0.0f);		
	parts = ft_split(params, ',');
	if (!parts)
		return (0);
	ok = 1;
	if (split_len(parts) != 3)
		ok = 0;
	else if (!get_normed_float(&colour.x, parts[0], 0, 255.0f) || !get_normed_float(&colour.y, parts[1], 0, 255.0f)
		|| !get_normed_float(&colour.z, parts[2], 0, 255.0f))
		ok = 0;
	*mat = init_lambertian(colour); 
	ft_free_split(parts);
	return (ok);
}

int	set_sphere(t_data *data, char **params)
{
	t_sphere	sphere;
	t_sphere	*object;				

	if (!split_count(params, 4))
		return (0);	
	if (!set_pts(&sphere.center, params[1], get_pt))
		return (0);
	if (!set_radius(&sphere.radius, params[2]))
		return (0);
	if (!set_material(&sphere.mat, params[3]))
		return (0);

	object = make_sphere(sphere);
	if (!object)
		return (return_print_error("Failed to allocate sphere.", 0));
	if (data->world.add(&data->world, object) != EXIT_SUCCESS)
	{
		free(object);
		return (return_print_error("Failed to add sphere to world.", 0));
	}
	return (1);
}

int	set_cylinder(t_data *data, char **params)
{
	t_cylinder	cylinder;
	t_cylinder	*object;


	if (!split_count(params, 6))
		return (0);
	if (!set_pts(&cylinder.center, params[1], get_pt))
		return (0);
	if (!set_pts(&cylinder.normal, params[2], get_uvec_pt))
		return (0);
	if (!set_radius(&cylinder.radius, params[3]))
		return (0);
	if (!set_height(&cylinder.height, params[4]))
		return (0);
	if (!set_material(&cylinder.mat, params[5]))
		return (0);
	object = make_cylinder(cylinder);
	if (!object)
		return (return_print_error("Failed to allocate cylinder.", 0));
	if (data->world.add(&data->world, object) != EXIT_SUCCESS)
	{
		free(object);
		return (return_print_error("Failed to add cylinder to world.", 0));
	}
	return (1);
}

int	set_plane(t_data *data, char **params)
{
	t_plane *object;
	t_vec3	center;
	t_vec3	normal;
	t_vec3	colour = make_vec(0.5, 0.5, 0.5);

	if (!split_count(params, 4))
		return (0);
	if(!set_pts(&center, params[1], get_pt))
		return (0);
	if(!set_pts(&normal,params[2], get_uvec_pt))
		return (0);
	// if(!set_colour(&colour, params[3]))
	// 	return (0);
	object = make_infinite_plane(center, normal, colour);
	if (!object)
		return (return_print_error("Failed to allocate plane.", 0));
	if (data->world.add(&data->world, object) != EXIT_SUCCESS)
	{
		free(object);
		return (return_print_error("Failed to add plane to world.", 0));
	}
	return (1);
}

static int	process_type(t_data *data, char **params)
{
	if (ft_strcmp(params[0], "C") == 0)
		return (set_cam(data, params));
	if (ft_strcmp(params[0], "A") == 0)
		return (set_ambient_light(data, params));
	if (ft_strcmp(params[0], "L") == 0)
		return (set_light(data, params));
	if (ft_strcmp(params[0], "sp") == 0)
		return (set_sphere(data, params));
	if (ft_strcmp(params[0], "pl") == 0)
		return (set_plane(data, params));
	if (ft_strcmp(params[0], "cy") == 0)
		return (set_cylinder(data, params));
	print_error("type not found");
	return (0);
}

int	process_line(t_data *data, char *line)
{
	char	**params;
	int		result;

	if (!line || line[0] == '\0')
		return (1);
	params = ft_split(line, ' ');
	if (!params || !params[0])
		return (0);
	result = process_type(data, params);
	ft_free_split(params);
	return (result);
}

int	parse_input(t_data *data, int ac, char **av)
{
	int		fd;
	char	*line;
	char	*trimmed;

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
		close(fd);
		return (0);
	}
	if (ft_vec_new(data->world.objects, 0, sizeof(void *)) < 0)
	{
		print_error("Failed to allocate world object list.");
		free(line);
		close(fd);
		return (0);
	}
	while (line)
	{
		trimmed = ft_strtrim(line, " \t\n");
		free(line);
		if (!trimmed)
		{
			close(fd);
			return (0);
		}
		if (!process_line(data, trimmed))
		{
			free(trimmed);
			close(fd);
			return (0);
		}
		free(trimmed);
		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}

/*

TODO: error process flow
TODO: shrink

*/
