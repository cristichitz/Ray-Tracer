/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movement.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 18:40:31 by timurray          #+#    #+#             */
/*   Updated: 2026/06/05 17:24:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MLX42/MLX42.h"
#include "movable.h"
#include "rt_cpu.h"

void	object_selector(mlx_key_data_t keydata, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (data->world.objects->len == 0)
		return ;
	if (keydata.key == MLX_KEY_C && keydata.action == MLX_RELEASE)
	{
		data->object_i = (data->object_i + 1) % data->world.objects->len;
		data->render_check = true;
	}
}

bool	move_cam(t_data *data, float *speed)
{
	bool	moved;

	moved = false;
	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(data->mlx);
	if (mlx_is_key_down(data->mlx, MLX_KEY_W))
		moved = (data->cam.center.z -= *speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		moved = (data->cam.center.z += *speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_A))
		moved = (data->cam.center.x -= *speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_D))
		moved = (data->cam.center.x += *speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_Q))
		moved = (data->cam.center.y -= *speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_E))
		moved = (data->cam.center.y += *speed, true);
	return (moved);
}

bool	move_object(t_data *data, float *speed)
{
	t_movable	*object;
	bool		moved;

	moved = false;
	if (data->world.objects->len > 0)
	{
		object = (t_movable *)ft_vec_get(data->world.objects,
				data->object_i);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_6))
			moved = (object->center.x += *speed, true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_4))
			moved = (object->center.x -= *speed, true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_5))
			moved = (object->center.z += *speed, true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_8))
			moved = (object->center.z -= *speed, true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_9))
			moved = (object->center.y += *speed, true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_7))
			moved = (object->center.y -= *speed, true);
	}
	return (moved);
}