/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:25 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:37:22 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static void	update_camera(t_data *data, cl_float3 step, int moved)
{
	int	animating;

	animating = data->phys.running;
	if (animating)
		data->scene_dirty = 1;
	if (moved || animating)
	{
		if (moved)
			data->cam_center = add(data->cam_center, step);
		data->frame_index = 0;
		update_view(data);
	}
}

void	game_loop(void *param)
{
	t_data		*data;
	cl_float3	step;
	int			moved;

	data = (t_data *)param;
	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
	{
		mlx_close_window(data->mlx);
		return ;
	}
	moved = handle_rotation(data);
	if (handle_movement(data, &step))
		moved = 1;
	update_camera(data, step, moved);
	physics_step(data);
	if (data->frame_index >= ACCUM_MAX)
		return ;
	render_frame(data);
	data->frame_index++;
}

static int	init_window(t_data *data)
{
	data->mlx = mlx_init(data->frame.width, data->frame.height,
			"GPU RT", true);
	if (!data->mlx)
	{
		puts(mlx_strerror(mlx_errno));
		return (0);
	}
	data->img = mlx_new_image(data->mlx, data->frame.width,
			data->frame.height);
	if (!data->img || mlx_image_to_window(data->mlx, data->img, 0, 0) == -1)
	{
		mlx_close_window(data->mlx);
		puts(mlx_strerror(mlx_errno));
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_data	data;

	memset(&data, 0, sizeof(data));
#ifndef METAL_GPU
	setup_local_cl();
#endif
	data.render_mode = render_mode_on(argc, argv);
	if (!load_scene(&data, argc, argv))
		return (free(data.objects), EXIT_FAILURE);
	initialize(&data);
	if (!init_window(&data))
		return (free(data.objects), EXIT_FAILURE);
	init_gpu(&data);
	if (data.render_mode)
		mlx_loop_hook(data.mlx, render_loop, &data);
	else
		mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, key_hook, &data);
	mlx_mouse_hook(data.mlx, mouse_hook, &data);
	mlx_resize_hook(data.mlx, resize_hook, &data);
	mlx_close_hook(data.mlx, cleanup, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}
