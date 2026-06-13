/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_mode.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 17:30:24 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 17:35:39 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "rt_cpu.h"

static void	render_mode(t_data *data)
{
	if (data->render_mode == RENDER_PATH_TRACE)
		data->render_mode = RENDER_DIRECT;
	else
		data->render_mode = RENDER_PATH_TRACE;
	data->render_check = true;
	set_quality(data, LOW);
	data->wait_frames = 0;
}

void	object_selector(mlx_key_data_t keydata, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (keydata.key == MLX_KEY_M && keydata.action == MLX_RELEASE)
		render_mode(data);
	if (data->world.objects->len == 0)
		return ;
	if (keydata.key == MLX_KEY_C && keydata.action == MLX_RELEASE)
	{
		data->object_i = (data->object_i + 1) % data->world.objects->len;
		data->render_check = true;
		if (data->object_i == data->light_i)
			ft_printf("Light: %u/%u\n", data->object_i + 1,
				data->world.objects->len);
		else
			ft_printf("Object: %u/%u\n", data->object_i + 1,
				data->world.objects->len);
	}
}
