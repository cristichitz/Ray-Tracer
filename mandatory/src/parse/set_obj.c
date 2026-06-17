/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_obj.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 12:59:55 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 20:59:27 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

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
	if (!force_normalised(&data->cam.uvec))
		return (0);
	if (!set_fov(&data->cam.fov, params[3]))
		return (0);
	data->set_cam = true;
	return (1);
}

int	set_light(t_data *data, char **params)
{
	size_t	len;

	len = split_len(params);
	if (len != 3 && len != 4)
		return (return_print_error("Invalid number of arguments.", 0));
	if (data->set_light == true)
	{
		print_error("Duplicate light entry.");
		return (0);
	}
	if (!set_pts(&data->light.center, params[1], get_pt))
		return (0);
	if (!set_brightness(&data->light.brightness, params[2]))
		return (0);
	data->light.colour = make_vec(1.0f, 1.0f, 1.0f);
	if (len == 4 && !set_colour(&data->light.colour, params[3]))
		return (0);
	if (!add_light_quad(data, data->light))
		return (0);
	data->set_light = true;
	return (1);
}

int	set_sphere(t_data *data, char **params)
{
	t_sphere	sphere;

	if (!split_count(params, 4))
		return (0);
	if (!set_pts(&sphere.center, params[1], get_pt))
		return (0);
	if (!set_radius(&sphere.radius, params[2]))
		return (0);
	if (!set_material(&sphere.mat, params[3]))
		return (0);
	return (add_to_world(data, make_sphere(sphere)));
}

int	set_cylinder(t_data *data, char **params)
{
	t_cylinder	cylinder;

	if (!split_count(params, 6))
		return (0);
	if (!set_pts(&cylinder.center, params[1], get_pt))
		return (0);
	if (!set_pts(&cylinder.normal, params[2], get_uvec_pt))
		return (0);
	if (!force_normalised(&cylinder.normal))
		return (0);
	if (!set_radius(&cylinder.radius, params[3]))
		return (0);
	if (!set_height(&cylinder.height, params[4]))
		return (0);
	if (!set_material(&cylinder.mat, params[5]))
		return (0);
	return (add_to_world(data, make_cylinder(cylinder)));
}

int	set_plane(t_data *data, char **params)
{
	t_plane	plane;
	t_vec3	colour;

	colour = make_vec(0.5f, 0.5f, 0.5f);
	if (!split_count(params, 4))
		return (0);
	if (!set_pts(&plane.q, params[1], get_pt))
		return (0);
	if (!set_pts(&plane.normal, params[2], get_uvec_pt))
		return (0);
	if (!force_normalised(&plane.normal))
		return (0);
	if (!set_colour(&colour, params[3]))
		return (0);
	plane.mat = init_lambertian(colour);
	return (add_to_world(data, make_plane(plane)));
}
