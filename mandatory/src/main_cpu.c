/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cpu.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:36:24 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 19:49:34 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "parse.h"
#include "rt_cpu.h"
#include <stdlib.h>

static void	render_scene(bool *scene_changed, t_data *data)
{
	if (scene_changed)
	{
		set_quality(data, LOW);
		data->wait_frames = 0;
	}
	else if (data->samples_per_pixel == 1)
	{
		data->wait_frames++;
		if (data->wait_frames > 15)
			set_quality(data, HIGH);
	}
	if (scene_changed || data->render_check)
	{
		update_viewport(data);
		if (data->render_mode == RENDER_DIRECT)
			render_frame_direct(data);
		else
			render_frame(data);
		data->render_check = false;
	}
}

void	game_loop(void *param)
{
	t_data	*data;
	float	speed;
	float	rotation_speed;
	bool	scene_changed;

	data = (t_data *)param;
	speed = 0.2f;
	rotation_speed = 0.03f;
	scene_changed = false;
	if (move_cam(data, &speed))
		scene_changed = true;
	if (rotate_cam(data, &rotation_speed))
		scene_changed = true;
	if (data->world.objects->len > 0)
	{
		if (move_object(data, &speed))
			scene_changed = true;
		if (resize_object(data, &speed))
			scene_changed = true;
		if (rotate_object(data, &rotation_speed))
			scene_changed = true;
	}
	render_scene(&scene_changed, data);
}

static int	init_window(t_data *data)
{
	data->mlx = mlx_init(data->width, data->height, "MiniRT", true);
	if (!data->mlx)
	{
		ft_printfd(2, "%s\n", mlx_strerror(mlx_errno));
		data->world.destroy(&data->world);
		return (EXIT_FAILURE);
	}
	data->img = mlx_new_image(data->mlx, data->width, data->height);
	if (!data->img || mlx_image_to_window(data->mlx, data->img, 0, 0) == -1)
	{
		mlx_close_window(data->mlx);
		ft_printfd(2, "%s\n", mlx_strerror(mlx_errno));
		data->world.destroy(&data->world);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int	main(int ac, char **av)
{
	t_data	data;
	t_list	obj;

	ft_memset(&data, 0, sizeof(data));
	ft_memset(&obj, 0, sizeof(obj));
	data.world.objects = &obj;
	if (init_world(&data.world))
		return (EXIT_FAILURE);
	if (!parse_input(&data, ac, av))
	{
		data.world.destroy(&data.world);
		return (EXIT_FAILURE);
	}
	init_viewport(&data);
	if (init_window(&data))
		return (EXIT_FAILURE);
	mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, &object_selector, &data);
	mlx_resize_hook(data.mlx, resize_hook, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	data.world.destroy(&data.world);
	return (EXIT_SUCCESS);
}
