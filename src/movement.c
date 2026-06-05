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
	if (keydata.key == MLX_KEY_C && keydata.action == MLX_RELEASE)
	{
		data->object_i++;
	}
}

void	move_cam(t_data *data, float *speed)
{
	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(data->mlx);
	if (mlx_is_key_down(data->mlx, MLX_KEY_W))
		data->cam.center.z -= *speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		data->cam.center.z += *speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_A))
		data->cam.center.x -= *speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_D))
		data->cam.center.x += *speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_Q))
		data->cam.center.y -= *speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_E))
		data->cam.center.y += *speed;
}

void	move_object(t_data *data, float *speed)
{
	t_movable *object;

	if (data->world.objects->len > 0)
	{
		object = (t_movable *)ft_vec_get(data->world.objects, data->object_i
				% data->world.objects->len);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_6))
			object->center.x += *speed;
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_4))
			object->center.x -= *speed;
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_5))
			object->center.z += *speed;
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_8))
			object->center.z -= *speed;
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_9))
			object->center.y += *speed;
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_7))
			object->center.y -= *speed;
	}	
}