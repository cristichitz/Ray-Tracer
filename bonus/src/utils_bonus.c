/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:54 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:38:54 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

float	random_float(float min, float max)
{
	return (min + (max - min) * (rand() / ((double)RAND_MAX + 1)));
}

float	degrees_to_radians(float degrees)
{
	return (degrees * M_PI / 180.0f);
}

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

void	key_hook(mlx_key_data_t key, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (key.action != MLX_PRESS || data->render_mode)
		return ;
	if (key.key == MLX_KEY_SPACE)
		shove_forward(data);
}

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
