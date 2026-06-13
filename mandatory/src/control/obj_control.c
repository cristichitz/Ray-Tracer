/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 18:40:31 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 17:35:07 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "rt_cpu.h"

static bool	adjust_brightness(t_data *data, float *scalar)
{
	bool	changed;

	changed = false;
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_ADD))
	{
		data->light.brightness += *scalar;
		if (data->light.brightness > 1.0f)
			data->light.brightness = 1.0f;
		changed = true;
	}
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_SUBTRACT))
	{
		data->light.brightness -= *scalar;
		if (data->light.brightness < 0.0f)
			data->light.brightness = 0.0f;
		changed = true;
	}
	return (changed);
}

bool	resize_object(t_data *data, float *scalar)
{
	t_hittable	*object;
	bool		resized;

	resized = false;
	object = (t_hittable *)ft_vec_get(data->world.objects, data->object_i);
	if (data->object_i == data->light_i)
		return (adjust_brightness(data, scalar));
	if (object->resize)
	{
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_ADD))
			resized = (object->resize(object, *scalar), true);
		if (mlx_is_key_down(data->mlx, MLX_KEY_KP_SUBTRACT))
			resized = (object->resize(object, -(*scalar)), true);
	}
	return (resized);
}

bool	rotate_object(t_data *data, float *rotation_speed)
{
	t_hittable	*object;
	t_vec3		right;
	t_vec3		up;
	bool		rotated;

	rotated = false;
	object = (t_hittable *)ft_vec_get(data->world.objects, data->object_i);
	if (!object->rotate)
		return (false);
	right = norm(cross(make_vec(0.0f, 1.0f, 0.0f), data->cam_forward));
	up = norm(cross(data->cam_forward, right));
	if (mlx_is_key_down(data->mlx, MLX_KEY_L))
		rotated = (object->rotate(object, up, *rotation_speed), true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_J))
		rotated = (object->rotate(object, up, -*rotation_speed), true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_I))
		rotated = (object->rotate(object, right, *rotation_speed), true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_K))
		rotated = (object->rotate(object, right, -*rotation_speed), true);
	return (rotated);
}
