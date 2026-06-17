/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:11:49 by timurray          #+#    #+#             */
/*   Updated: 2026/06/16 11:14:15 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

static void	clean_gnl(int fd)
{
	char	*line;

	line = get_next_line(fd);
	while (line)
	{
		free(line);
		line = get_next_line(fd);
	}
}

static int	preprocess_line(t_data *data, int fd, char **line)
{
	char	*trimmed;

	trimmed = ft_strtrim(*line, " \t\n");
	free(*line);
	*line = NULL;
	if (trimmed)
		replace_tabs(trimmed);
	if (!trimmed || !process_line(data, trimmed))
	{
		free(trimmed);
		clean_gnl(fd);
		return (0);
	}
	free(trimmed);
	*line = get_next_line(fd);
	return (1);
}

int	process_scene(t_data *data, int fd)
{
	char	*line;

	line = get_next_line(fd);
	if (!line)
		return (return_print_error("Empty file.", 0));
	if (ft_vec_new(data->world.objects, 0, sizeof(void *)) < 0)
	{
		free(line);
		return (return_print_error("Failed to allocate world object list.", 0));
	}
	while (line)
	{
		if (preprocess_line(data, fd, &line) == 0)
			return (0);
	}
	return (1);
}

int	valid_scene(t_data *data)
{
	if (!data->set_cam)
		return (return_print_error("Missing camera (C).", 0));
	if (!data->set_light)
		return (return_print_error("Missing light (L).", 0));
	return (1);
}

int	add_to_world(t_data *data, void *object)
{
	if (!object)
		return (return_print_error("Failed to allocate object.", 0));
	if (data->world.add(&data->world, object) != EXIT_SUCCESS)
	{
		if (((t_hittable *)object)->destroy)
			((t_hittable *)object)->destroy(object);
		free(object);
		return (return_print_error("Failed to add object to world.", 0));
	}
	return (1);
}
