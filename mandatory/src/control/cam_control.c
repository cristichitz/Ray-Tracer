/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cam_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 15:23:10 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 17:25:52 by cdohanic         ###   ########.fr       */
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
		step = add(step, scale(data->cam_forward, *speed));
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		step = add(step, scale(data->cam_forward, -*speed));
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
	bool	rotated;

	rotated = false;
	if (mlx_is_key_down(data->mlx, MLX_KEY_LEFT))
		rotated = (data->cam.yaw -= *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_RIGHT))
		rotated = (data->cam.yaw += *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_UP))
		rotated = (data->cam.pitch += *rotation_speed, true);
	if (mlx_is_key_down(data->mlx, MLX_KEY_DOWN))
		rotated = (data->cam.pitch -= *rotation_speed, true);
	if (data->cam.pitch > (float)M_PI / 2.0f - 0.01f)
		data->cam.pitch = (float)M_PI / 2.0f - 0.01f;
	if (data->cam.pitch < (float)-M_PI / 2.0f + 0.01f)
		data->cam.pitch = (float)-M_PI / 2.0f + 0.01f;
	return (rotated);
}

void	camera_setup(t_data *data)
{
	t_vec3	world_up;
	t_vec3	right;
	t_vec3	up;

	world_up = make_vec(0.0f, 1.0f, 0.0f);
	data->cam_forward.x = sinf(data->cam.yaw) * cosf(data->cam.pitch);
	data->cam_forward.y = sinf(data->cam.pitch);
	data->cam_forward.z = cosf(data->cam.yaw) * cosf(data->cam.pitch);
	data->cam_forward = norm(data->cam_forward);
	right = cross(world_up, data->cam_forward);
	if (length_squared(right) < 1e-8f)
		right = make_vec(1.0f, 0.0f, 0.0f);
	right = norm(right);
	up = cross(data->cam_forward, right);
	data->horizontal = scale(right, data->viewport_w);
	data->vertical = scale(up, -data->viewport_h);
	data->px_w = scale(data->horizontal, 1.0f / (float)data->width);
	data->px_h = scale(data->vertical, 1.0f / (float)data->height);
}

void	update_viewport(t_data *data)
{
	t_vec3	upper_left_corner;

	camera_setup(data);
	data->origin = make_vec(data->cam.center.x, data->cam.center.y,
			data->cam.center.z);
	upper_left_corner = sub(data->origin, scale(data->horizontal, 0.5f));
	upper_left_corner = sub(upper_left_corner, scale(data->vertical, 0.5f));
	upper_left_corner = add(upper_left_corner, scale(data->cam_forward,
				data->focal_length));
	data->pixel00_loc = add(upper_left_corner, scale(add(data->px_w,
					data->px_h), 0.5f));
}

void	init_viewport(t_data *data)
{
	t_vec3	dir;

	data->width = WIDTH;
	data->height = (int)(data->width / (16.0 / 9.0));
	if (data->height < 1)
		data->height = 1;
	data->aspect_ratio = (float)data->width / (float)data->height;
	data->viewport_h = 2.0f * tanf(deg_to_rad((float)data->cam.fov) / 2.0f);
	data->viewport_w = data->aspect_ratio * data->viewport_h;
	data->focal_length = 1.0f;
	set_quality(data, LOW);
	dir = norm(data->cam.uvec);
	data->cam.yaw = atan2f(dir.x, dir.z);
	data->cam.pitch = asinf(clampf(dir.y, -1.0f, 1.0f));
	data->object_i = 0;
	data->wait_frames = 0;
	data->render_check = true;
	data->render_mode = RENDER_PATH_TRACE;
	update_viewport(data);
}
