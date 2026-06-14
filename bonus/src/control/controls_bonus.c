/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:32 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 18:22:20 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

#define MOVE_SPEED 0.3f
#define ROT_SPEED 0.05f

static void	clamp_pitch(t_data *data)
{
	if (data->cam_pitch > (float)CL_M_PI / 2.0f - 0.01f)
		data->cam_pitch = (float)CL_M_PI / 2.0f - 0.01f;
	if (data->cam_pitch < (float)-CL_M_PI / 2.0f + 0.01f)
		data->cam_pitch = (float)-CL_M_PI / 2.0f + 0.01f;
}

int	handle_rotation(t_data *data)
{
	int	moved;

	moved = 0;
	if (mlx_is_key_down(data->mlx, MLX_KEY_LEFT))
	{
		data->cam_yaw -= ROT_SPEED;
		moved = 1;
	}
	if (mlx_is_key_down(data->mlx, MLX_KEY_RIGHT))
	{
		data->cam_yaw += ROT_SPEED;
		moved = 1;
	}
	if (mlx_is_key_down(data->mlx, MLX_KEY_UP))
	{
		data->cam_pitch += ROT_SPEED;
		moved = 1;
	}
	if (mlx_is_key_down(data->mlx, MLX_KEY_DOWN))
	{
		data->cam_pitch -= ROT_SPEED;
		moved = 1;
	}
	clamp_pitch(data);
	return (moved);
}

static int	move_step(t_data *data, cl_float3 *step, cl_float3 dir, int key)
{
	if (!mlx_is_key_down(data->mlx, key))
		return (0);
	*step = add(*step, scale(dir, MOVE_SPEED));
	return (1);
}

int	handle_movement(t_data *data, cl_float3 *step)
{
	cl_float3	fwd;
	cl_float3	right;
	cl_float3	up;
	int			moved;

	fwd = data->cam_dir;
	right = norm(cross(make_float3(0.0f, 1.0f, 0.0f), fwd));
	up = norm(cross(fwd, right));
	*step = make_float3(0.0f, 0.0f, 0.0f);
	moved = move_step(data, step, fwd, MLX_KEY_W);
	moved = move_step(data, step, scale(fwd, -1.0f), MLX_KEY_S) || moved;
	moved = move_step(data, step, scale(right, -1.0f), MLX_KEY_A) || moved;
	moved = move_step(data, step, right, MLX_KEY_D) || moved;
	moved = move_step(data, step, up, MLX_KEY_E) || moved;
	moved = move_step(data, step, scale(up, -1.0f), MLX_KEY_Q) || moved;
	return (moved);
}
