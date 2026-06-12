/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:10:56 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:30:31 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	process_type(t_data *data, char **params)
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
	if (!params)
		return (0);
	if (!params[0])
	{
		ft_free_split(params);
		return (1);
	}
	result = process_type(data, params);
	ft_free_split(params);
	return (result);
}
