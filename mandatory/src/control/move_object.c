/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_object.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 17:35:02 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/16 15:36:11 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "rt_cpu.h"

static bool	key_down2(t_data *data, int key1, int key2)
{
	return (mlx_is_key_down(data->mlx, key1)
		|| mlx_is_key_down(data->mlx, key2));
}

static t_vec3	move_step(t_data *data, float *speed)
{
	t_vec3	right;
	t_vec3	up;
	t_vec3	step;

	right = norm(cross(make_vec(0.0f, 1.0f, 0.0f), data->cam_forward));
	up = norm(cross(data->cam_forward, right));
	step = make_vec(0.0f, 0.0f, 0.0f);
	if (key_down2(data, MLX_KEY_KP_6, MLX_KEY_H))
		step = add(step, scale(right, *speed));
	if (key_down2(data, MLX_KEY_KP_4, MLX_KEY_F))
		step = add(step, scale(right, -*speed));
	if (key_down2(data, MLX_KEY_KP_5, MLX_KEY_G))
		step = add(step, scale(data->cam_forward, -*speed));
	if (key_down2(data, MLX_KEY_KP_8, MLX_KEY_T))
		step = add(step, scale(data->cam_forward, *speed));
	if (key_down2(data, MLX_KEY_KP_9, MLX_KEY_Y))
		step = add(step, scale(up, *speed));
	if (key_down2(data, MLX_KEY_KP_7, MLX_KEY_R))
		step = add(step, scale(up, -*speed));
	return (step);
}

bool	move_object(t_data *data, float *speed)
{
	t_movable	*object;
	t_vec3		step;

	step = move_step(data, speed);
	if (step.x == 0.0f && step.y == 0.0f && step.z == 0.0f)
		return (false);
	object = (t_movable *)ft_vec_get(data->world.objects, data->object_i);
	object->center = add(object->center, step);
	if (data->object_i == data->light_i)
		data->light.center = add(data->light.center, step);
	return (true);
}
