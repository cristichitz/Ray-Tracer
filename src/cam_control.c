/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cam_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:23:10 by timurray          #+#    #+#             */
/*   Updated: 2026/06/07 17:00:24 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

bool	move_cam(t_data *data, float *speed)
{
	t_vec3	up;
	t_vec3	right;
	t_vec3	step;

	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(data->mlx);
	right = norm(cross(make_vec(0.0f, 1.0f, 0.0f), data->cam_forward));
	up = norm(cross(data->cam_forward, right));
	step = make_vec(0.0f, 0.0f, 0.0f);
	if (mlx_is_key_down(data->mlx, MLX_KEY_W))
		step = add(step, scale(data->cam_forward, -*speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		step = add(step, scale(data->cam_forward, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_A))
		step = add(step, scale(right, -*speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_D))
		step = add(step, scale(right, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_Q))
		step = add(step, scale(up, -*speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_E))
		step = add(step, scale(up, *speed));
	if (step.x == 0.0f && step.y == 0.0f && step.z == 0.0f)
		return (false);
	data->cam.center = add(data->cam.center, step);
	return (true);
}

bool	rotate_cam(t_data *data, float *rotation_speed)
{
	bool rotated;

	rotated = false;
	if (mlx_is_key_down(data->mlx, MLX_KEY_LEFT))
		rotated = (data->cam.yaw += *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_RIGHT))
		rotated = (data->cam.yaw -= *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_UP))
		rotated = (data->cam.pitch -= *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_DOWN))
		rotated = (data->cam.pitch += *rotation_speed, true);
	if (data->cam.pitch > (float)M_PI / 2.0f - 0.01f)
		data->cam.pitch = (float)M_PI / 2.0f - 0.01f;
	if (data->cam.pitch < (float)-M_PI / 2.0f + 0.01f)
		data->cam.pitch = (float)-M_PI / 2.0f + 0.01f;
	return (rotated);
}
