/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 18:40:31 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 21:32:58 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

bool	move_object(t_data *data, float *speed)
{
	t_movable	*object;
	t_vec3		right;
	t_vec3		up;
	t_vec3		step;

	right = norm(cross(make_vec(0.0f, 1.0f, 0.0f), data->cam_forward));
	up = norm(cross(data->cam_forward, right));
	step = make_vec(0.0f, 0.0f, 0.0f);
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_6) || mlx_is_key_down(data->mlx, MLX_KEY_H))
		step = add(step, scale(right, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_4) || mlx_is_key_down(data->mlx, MLX_KEY_F))
		step = add(step, scale(right, -*speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_5) || mlx_is_key_down(data->mlx, MLX_KEY_G))
		step = add(step, scale(data->cam_forward, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_8) || mlx_is_key_down(data->mlx, MLX_KEY_T))
		step = add(step, scale(data->cam_forward, -*speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_9) || mlx_is_key_down(data->mlx, MLX_KEY_Y))
		step = add(step, scale(up, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_KP_7) || mlx_is_key_down(data->mlx, MLX_KEY_R))
		step = add(step, scale(up, -*speed));
	if (step.x == 0.0f && step.y == 0.0f && step.z == 0.0f)
		return (false);
	object = (t_movable *)ft_vec_get(data->world.objects, data->object_i);
	object->center = add(object->center, step);
	return (true);
}

bool	resize_object(t_data *data, float *scalar)
{
	t_hittable	*object;
	bool		resized;

	resized = false;
	object = (t_hittable *)ft_vec_get(data->world.objects, data->object_i);
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
