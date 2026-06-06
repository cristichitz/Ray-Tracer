/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cpu.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:36:24 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 17:54:13 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "parse.h"
#include "rt_cpu.h"

static void	update_viewport(t_data *data)
{
	t_vec3	upper_left_corner;

	data->origin = make_vec(data->cam.center.x, data->cam.center.y,
			data->cam.center.z);
	upper_left_corner = sub(data->origin, scale(data->horizontal, 0.5f));
	upper_left_corner = sub(upper_left_corner, scale(data->vertical, 0.5f));
	upper_left_corner = sub(upper_left_corner, make_vec(0.0f, 0.0f,
				data->focal_length));
	data->pixel00_loc = add(upper_left_corner, scale(add(data->px_w,
					data->px_h), 0.5f));
}

void	game_loop(void *param)
{
	t_data	*data;
	float	speed;
	bool	scene_changed;

	data = (t_data *)param;
	speed = 0.2f;
	scene_changed = false;
	if (move_cam(data, &speed))
		scene_changed = true;
	if (move_object(data, &speed))
		scene_changed = true;
	if (resize_object(data, &speed))
		scene_changed = true;
	if (scene_changed)
		set_quality(data, LOW);
	else if (data->samples_per_pixel == 1)
		set_quality(data, HIGH);
	if (scene_changed || data->render_check)
	{
		update_viewport(data);
		render_frame(data);
		data->render_check = false;
	}
}

void	initialize(t_data *data)
{
	data->width = WIDTH;
	data->height = (int)(data->width / (16.0 / 9.0));
	if (data->height < 1)
		data->height = 1;
	data->aspect_ratio = (float)data->width / (float)data->height;
	data->viewport_h = 2.0f * tanf(deg_to_rad((float)data->cam.fov) / 2.0f);
	data->viewport_w = data->aspect_ratio * data->viewport_h;
	data->focal_length = 1.0f;
	set_quality(data, HIGH);
	data->horizontal = make_vec(data->viewport_w, 0.0f, 0.0f);
	data->vertical = make_vec(0.0f, -data->viewport_h, 0.0f);
	data->px_w = scale(data->horizontal, (float)1 / (float)data->width);
	data->px_h = scale(data->vertical, (float)1 / (float)data->height);
	data->object_i = 0;
	data->render_check = true;
	update_viewport(data);
}

int	main(int ac, char **av)
{
	t_data			data;
	t_hittable_list	world;
	t_list			obj;

	data.set_ambient_light = false;
	data.set_cam = false;
	data.set_light = false;
	world.objects = &obj;
	if (init_world(&world))
		return (EXIT_FAILURE);
	data.world = world;
	if (ac != 2)
	{
		print_error("No scene file given.");
		return (0);
	}
	if (!parse_input(&data, ac, av))
	{
		// TODO: free memory
		return (EXIT_FAILURE);
	}
	initialize(&data);
	data.mlx = mlx_init(data.width, data.height, "CPU RT", true);
	if (!data.mlx)
	{
		puts(mlx_strerror(mlx_errno));
		return (EXIT_FAILURE);
	}
	data.img = mlx_new_image(data.mlx, data.width, data.height);
	if (!data.img)
	{
		mlx_close_window(data.mlx);
		puts(mlx_strerror(mlx_errno));
		return (EXIT_FAILURE);
	}
	if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1)
	{
		mlx_close_window(data.mlx);
		puts(mlx_strerror(mlx_errno));
		return (EXIT_FAILURE);
	}
	mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, &object_selector, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}
