/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_lights.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:06:37 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:33:10 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	set_ambient_light(t_data *data, char **params)
{
	t_vec3	ambient;

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
	data->set_ambient_light = true;
	return (1);
}
