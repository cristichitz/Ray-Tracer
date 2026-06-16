/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:25 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 14:40:28 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (width <= 0 || height <= 0)
		return ;
	data->frame.width = width;
	data->frame.height = height;
	data->frame.aspect_ratio = (float)data->frame.width
		/ (float)data->frame.height;
	data->frame.viewport_height = 2.0f
		* tanf(degrees_to_radians(data->cam_fov) / 2.0f);
	data->frame.viewport_width = data->frame.aspect_ratio
		* data->frame.viewport_height;
	mlx_resize_image(data->img, (uint32_t)width, (uint32_t)height);
	data->frame_index = 0;
	update_view(data);
}

/*
** SPACE shoves whatever dynamic body the camera is looking at along the view
** direction (the generic "launch"). Edge-triggered so one press = one shove.
** This is the only key hook the physics needs; a mouse-click impulse would be
** wired the same way (see physics_input_bonus.c).
*/
void	key_hook(mlx_key_data_t key, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (key.action != MLX_PRESS || data->render_mode)
		return ;
	if (key.key == MLX_KEY_SPACE)
		shove_forward(data);
}

/*
** Left click shoves the body under the cursor straight ahead (along the view
** direction). Picking + impulse reuse the modular physics_input helpers, so
** this hook is the whole feature; aim, click, watch it fly.
*/
void	mouse_hook(mouse_key_t button, action_t action,
		modifier_key_t mods, void *param)
{
	t_data	*data;
	t_ray	ray;
	int		x;
	int		y;
	int		i;

	(void)mods;
	data = (t_data *)param;
	if (button != MLX_MOUSE_BUTTON_LEFT || action != MLX_PRESS)
		return ;
	mlx_get_mouse_pos(data->mlx, &x, &y);
	ray = ray_from_screen(data, (float)x, (float)y);
	i = pick_body(data, ray);
	if (i < 0)
		return ;
	apply_impulse(&data->phys.bodies[i],
		scale(data->cam_dir, PHYS_CLICK_IMPULSE), data->phys.bodies[i].pos);
	data->phys.running = 1;
	data->phys.settle = 0;
}

/*
** Camera moved or the simulation is live: restart path-tracing accumulation
** and flag the scene dirty so the moved geometry re-uploads to the GPU.
*/
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
		cleanup(data);
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
	mlx_mouse_hook(data.mlx, mouse_hook, &data);
	mlx_resize_hook(data.mlx, resize_hook, &data);
	mlx_close_hook(data.mlx, cleanup, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}
