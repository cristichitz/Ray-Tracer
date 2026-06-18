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
#ifdef METAL_GPU
	if (data->render_mode == RENDER_DIRECT)
		data->render_mode = RENDER_PATH_TRACE;
	else if (data->render_mode == RENDER_PATH_TRACE)
		data->render_mode = RENDER_METAL;
	else
		data->render_mode = RENDER_DIRECT;
#else
	if (data->render_mode == RENDER_PATH_TRACE)
		data->render_mode = RENDER_DIRECT;
	else
		data->render_mode = RENDER_PATH_TRACE;
#endif
	data->render_check = true;
	set_quality(data, LOW);
	data->wait_frames = 0;
}

static void	toggle_material(t_data *data)
{
	t_hittable	*obj;

	if (data->render_mode != RENDER_PATH_TRACE)
		return ;
	obj = (t_hittable *)ft_vec_get(data->world.objects, data->object_i);
	if (!obj->material)
		return ;
	obj->material(obj);
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
	if (keydata.key == MLX_KEY_N && keydata.action == MLX_RELEASE)
		toggle_material(data);
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
