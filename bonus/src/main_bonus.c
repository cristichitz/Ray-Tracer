/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:25 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:51:28 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** SPACE scrambles, ENTER solves. Edge-triggered via the key hook so one press
** queues exactly one batch of moves (mlx_is_key_down would fire every frame).
*/
void	key_hook(mlx_key_data_t key, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (key.action != MLX_PRESS)
		return ;
	if (data->render_mode || data->phys.running)
		return ;
	if (key.key == MLX_KEY_SPACE)
		scramble_rubik(&data->rubik);
	else if (key.key == MLX_KEY_ENTER)
		solve_rubik(&data->rubik);
	else if (key.key == MLX_KEY_R)
		start_explode(data);
}

/*
** Camera moved or an animation is running: restart path tracing accumulation.
*/
static void	update_camera(t_data *data, cl_float3 step, int moved)
{
	if (moved || data->rubik.active || data->rubik.explode_active
		|| data->rubik.orbit_active || data->phys.running)
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
		cleanup(data);
		mlx_close_window(data->mlx);
		return ;
	}
	moved = handle_rotation(data);
	if (handle_movement(data, &step))
		moved = 1;
	update_camera(data, step, moved);
	step_rubik(data);
	physics_step(data);
	update_stage(data);
	render_frame(data);
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

	setup_local_cl();
	memset(&data, 0, sizeof(data));
	data.render_mode = render_mode_on(argc, argv);
	if (!load_scene(&data, argc, argv))
		return (EXIT_FAILURE);
	initialize(&data);
	if (!init_window(&data))
		return (EXIT_FAILURE);
	init_gpu(&data);
	if (data.render_mode)
		mlx_loop_hook(data.mlx, render_loop, &data);
	else
		mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, key_hook, &data);
	mlx_close_hook(data.mlx, cleanup, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}
