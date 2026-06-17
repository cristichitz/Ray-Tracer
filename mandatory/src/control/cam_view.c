/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cam_view.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 19:36:33 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 19:37:36 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

void	camera_setup(t_data *data)
{
	t_vec3	right;
	t_vec3	up;

	data->cam_forward.x = sinf(data->cam.yaw) * cosf(data->cam.pitch);
	data->cam_forward.y = sinf(data->cam.pitch);
	data->cam_forward.z = cosf(data->cam.yaw) * cosf(data->cam.pitch);
	data->cam_forward = norm(data->cam_forward);
	right = safe_right_vec(data->cam_forward);
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
	data->viewport_h = 2.0f * tanf(deg_to_rad(clampf((float)data->cam.fov,
					1.0f, 179.0f)) / 2.0f);
	data->viewport_w = data->aspect_ratio * data->viewport_h;
	data->focal_length = 1.0f;
	set_quality(data, LOW);
	dir = norm(data->cam.uvec);
	data->cam.yaw = atan2f(dir.x, dir.z);
	data->cam.pitch = asinf(clampf(dir.y, -1.0f, 1.0f));
	data->object_i = 0;
	data->wait_frames = 0;
	data->render_check = true;
	data->render_mode = RENDER_DIRECT;
	update_viewport(data);
}
