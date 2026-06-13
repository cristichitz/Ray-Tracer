/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cpu.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:36:24 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 17:28:43 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "movable.h"
#include "parse.h"
#include "rt_cpu.h"
#include <stdlib.h>

static void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (width <= 0 || height <= 0)
		return ;
	data->width = (uint32_t)width;
	data->height = (uint32_t)height;
	data->aspect_ratio = (float)data->width / (float)data->height;
	data->viewport_h = 2.0f * tanf(deg_to_rad((float)data->cam.fov) / 2.0f);
	data->viewport_w = data->aspect_ratio * data->viewport_h;
	mlx_resize_image(data->img, data->width, data->height);
	set_quality(data, LOW);
	data->wait_frames = 0;
	data->render_check = true;
}

//TODO: too big.
void	game_loop(void *param)
{
	t_data	*data;
	float	speed;
	float	rotation_speed;
	bool	scene_changed;

	data = (t_data *)param;
	speed = 0.2f;
	rotation_speed = 0.03f;
	scene_changed = false;
	if (move_cam(data, &speed))
		scene_changed = true;
	if (rotate_cam(data, &rotation_speed))
		scene_changed = true;
	if (data->world.objects->len > 0)
	{
		if (move_object(data, &speed))
			scene_changed = true;
		if (resize_object(data, &speed))
			scene_changed = true;
		if (rotate_object(data, &rotation_speed))
			scene_changed = true;
	}
	if (scene_changed)
	{
		set_quality(data, LOW);
		data->wait_frames = 0;
	}
	else if (data->samples_per_pixel == 1)
	{
		data->wait_frames++;
		if (data->wait_frames > 15)
			set_quality(data, HIGH);
	}
	if (scene_changed || data->render_check)
	{
		update_viewport(data);
		if (data->render_mode == RENDER_DIRECT)
			render_frame_direct(data);
		else
			render_frame(data);
		data->render_check = false;
	}
}

//TODO: Make a seperate file for this or leave for bonus?
void	make_cornell_box(t_hittable_list *world)
{
	t_material	red;
	t_material	white;
	t_material	green;
	t_material	difflight;

	red = init_lambertian(make_vec(0.65f, 0.05f, 0.05f));
	white = init_lambertian(make_vec(0.73f, 0.73f, 0.73f));
	green = init_lambertian(make_vec(0.12f, 0.45f, 0.15f));
	difflight = init_diffuse_light(make_vec(15.0f, 15.0f, 15.0f));
	world->add(world, make_quad(make_vec(555, 0, 0), make_vec(0, 555, 0),
			make_vec(0, 0, 555), green));
	world->add(world, make_quad(make_vec(0, 0, 0), make_vec(0, 555, 0),
			make_vec(0, 0, 555), red));
	world->add(world, make_quad(make_vec(343, 554, 332), make_vec(-130, 0, 0),
			make_vec(0, 0, -105), difflight));
	world->add(world, make_quad(make_vec(0, 0, 0), make_vec(555, 0, 0),
			make_vec(0, 0, 555), white));
	world->add(world, make_quad(make_vec(555, 555, 555), make_vec(-555, 0, 0),
			make_vec(0, 0, -555), white));
	world->add(world, make_quad(make_vec(0, 0, 555), make_vec(555, 0, 0),
			make_vec(0, 555, 0), white));
}

static int	init_window(t_data *data)
{
	data->mlx = mlx_init(data->width, data->height, "MiniRT", true);
	if (!data->mlx)
	{
		ft_printfd(2, "%s\n", mlx_strerror(mlx_errno));
		data->world.destroy(&data->world);
		return (EXIT_FAILURE);
	}
	data->img = mlx_new_image(data->mlx, data->width, data->height);
	if (!data->img || mlx_image_to_window(data->mlx, data->img, 0, 0) == -1)
	{
		mlx_close_window(data->mlx);
		ft_printfd(2, "%s\n", mlx_strerror(mlx_errno));
		data->world.destroy(&data->world);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

// Redundant because of memset
// static void	init_defaults(t_data *data)
// {
// 	data->set_ambient_light = false;
// 	data->set_cam = false;
// 	data->set_light = false;
// 	data->headless = false;
// 	data->fb = NULL;
// }

// TODO: too many vars
int	main(int ac, char **av)
{
	t_data			data;
	t_list			obj;

	ft_memset(&data, 0, sizeof(data));
	ft_memset(&obj, 0, sizeof(obj));
	data.world.objects = &obj;
	if (init_world(&data.world))
		return (EXIT_FAILURE);
	if (!parse_input(&data, ac, av))
	{
		data.world.destroy(&data.world);
		return (EXIT_FAILURE);
	}
	init_viewport(&data);
	if (init_window(&data))
		return (EXIT_FAILURE);
	mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, &object_selector, &data);
	mlx_resize_hook(data.mlx, resize_hook, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	data.world.destroy(&data.world);
	return (EXIT_SUCCESS);
}
