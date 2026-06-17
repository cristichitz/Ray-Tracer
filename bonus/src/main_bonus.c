/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:51:25 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 14:40:28 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (width <= 0 || height <= 0)
		return ;
	data->frame.width = width;
	data->frame.height = height;
	data->frame.aspect_ratio = (float)data->frame.width
		/ (float)data->frame.height;
	data->frame.viewport_height = 2.0f
		* tanf(degrees_to_radians(data->cam_fov) / 2.0f);
	data->frame.viewport_width = data->frame.aspect_ratio
		* data->frame.viewport_height;
	mlx_resize_image(data->img, (uint32_t)width, (uint32_t)height);
	data->frame_index = 0;
	update_view(data);
}

/*
** M toggles first-person character mode (only once a player body exists). In
** free-fly mode SPACE shoves whatever body the camera looks at; in character
** mode SPACE is the jump (handled each frame in game_loop), so it is gated off
** here. Edge-triggered so one press = one action.
*/
void	key_hook(mlx_key_data_t key, void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (key.action != MLX_PRESS || data->render_mode)
		return ;
	if (key.key == MLX_KEY_M && data->phys.character >= 0)
		data->char_mode = !data->char_mode;
	if (key.key == MLX_KEY_SPACE && !data->char_mode)
		shove_forward(data);
}

/* World-space ray through the current mouse cursor position. */
static t_ray	mouse_ray(t_data *data)
{
	int	x;
	int	y;

	mlx_get_mouse_pos(data->mlx, &x, &y);
	return (ray_from_screen(data, (float)x, (float)y));
}

/* Push the sphere under the cursor straight ahead (the old click behaviour). */
static void	mouse_shove(t_data *data, int i)
{
	apply_impulse(&data->phys.bodies[i],
		scale(data->cam_dir, PHYS_CLICK_IMPULSE), data->phys.bodies[i].pos);
	data->phys.running = 1;
	data->phys.settle = 0;
}

/* Pick up the box under the cursor; it then floats along the mouse ray. */
static void	mouse_grab(t_data *data, int i, t_ray ray)
{
	cl_float3	off;

	off = sub(data->phys.bodies[i].pos, ray.origin);
	data->phys.held = i;
	data->phys.bodies[i].held = 1;
	data->phys.bodies[i].sleeping = 0;
	data->phys.hold_dist = sqrtf(length_squared(off));
	data->phys.running = 1;
	data->phys.settle = 0;
}

/* Middle click while holding: fling the body along the cursor ray with spin. */
static void	throw_held(t_data *data)
{
	t_rbody	*b;
	t_ray	ray;

	b = &data->phys.bodies[data->phys.held];
	ray = mouse_ray(data);
	b->held = 0;
	data->phys.held = -1;
	b->vel = scale(ray.dir, PHYS_LAUNCH_SPEED);
	b->omega = scale(cross(ray.dir, make_float3(0.0f, 1.0f, 0.0f)), 12.0f);
	b->sleeping = 0;
	data->phys.running = 1;
	data->phys.settle = 0;
}

/*
** Mouse does it all. While holding a body: middle click throws it (velocity +
** spin), left click just drops it. Otherwise a left click aiming at a box
** grabs it (it then follows the cursor), at a sphere shoves it, and at a wall
** drops a portal there.
*/
void	mouse_hook(mouse_key_t button, action_t action,
		modifier_key_t mods, void *param)
{
	t_data	*data;
	t_ray	ray;
	int		i;

	(void)mods;
	data = (t_data *)param;
	if (action != MLX_PRESS || data->render_mode)
		return ;
	if (data->phys.held >= 0)
	{
		if (button == MLX_MOUSE_BUTTON_MIDDLE)
			throw_held(data);
		else if (button == MLX_MOUSE_BUTTON_LEFT)
		{
			data->phys.bodies[data->phys.held].held = 0;
			data->phys.held = -1;
		}
		return ;
	}
	if (button != MLX_MOUSE_BUTTON_LEFT)
		return ;
	ray = mouse_ray(data);
	i = pick_body(data, ray);
	if (i < 0)
		place_portal(data, ray);
	else if (data->phys.bodies[i].shape == 0)
		mouse_grab(data, i, ray);
	else
		mouse_shove(data, i);
}

/*
** Scroll wheel reels the held cube along the cursor ray: wheel up pushes it
** further away, wheel down pulls it closer, so it is easy to aim and drop.
*/
void	scroll_hook(double xdelta, double ydelta, void *param)
{
	t_data	*data;

	(void)xdelta;
	data = (t_data *)param;
	if (data->phys.held < 0 || data->render_mode)
		return ;
	data->phys.hold_dist += (float)ydelta * 2.0f;
	if (data->phys.hold_dist < 3.0f)
		data->phys.hold_dist = 3.0f;
	if (data->phys.hold_dist > 200.0f)
		data->phys.hold_dist = 200.0f;
}

/*
** Camera moved or the simulation is live: restart path-tracing accumulation
** and flag the scene dirty so the moved geometry re-uploads to the GPU.
*/
static void	update_camera(t_data *data, cl_float3 step, int moved)
{
	int	animating;

	animating = data->phys.running;
	if (animating)
		data->scene_dirty = 1;
	if (moved || animating)
	{
		if (moved)
			data->cam_center = add(data->cam_center, step);
		data->frame_index = 0;
		update_view(data);
	}
}

void	game_loop(void *param)
{
	t_data		*data;
	cl_float3	step;
	int			moved;

	data = (t_data *)param;
	step = make_float3(0.0f, 0.0f, 0.0f);
	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
	{
		cleanup(data);
		mlx_close_window(data->mlx);
		return ;
	}
	moved = handle_rotation(data);
	if (data->char_mode)
		character_input(data);
	else if (handle_movement(data, &step))
		moved = 1;
	update_camera(data, step, moved);
	physics_step(data);
	if (data->char_mode)
		character_camera(data);
	if (data->frame_index >= ACCUM_MAX)
		return ;
	render_frame(data);
	data->frame_index++;
}

static int	init_window(t_data *data)
{
	data->mlx = mlx_init(data->frame.width, data->frame.height,
			"GPU RT", true);
	if (!data->mlx)
	{
		puts(mlx_strerror(mlx_errno));
		return (0);
	}
	data->img = mlx_new_image(data->mlx, data->frame.width,
			data->frame.height);
	if (!data->img || mlx_image_to_window(data->mlx, data->img, 0, 0) == -1)
	{
		mlx_close_window(data->mlx);
		puts(mlx_strerror(mlx_errno));
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_data	data;

	setup_local_cl();
	memset(&data, 0, sizeof(data));
	data.render_mode = render_mode_on(argc, argv);
	if (!load_scene(&data, argc, argv))
		return (EXIT_FAILURE);
	reserve_portals(&data);
	spawn_character(&data);
	initialize(&data);
	if (!init_window(&data))
		return (EXIT_FAILURE);
	init_gpu(&data);
	if (data.render_mode)
		mlx_loop_hook(data.mlx, render_loop, &data);
	else
		mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_key_hook(data.mlx, key_hook, &data);
	mlx_mouse_hook(data.mlx, mouse_hook, &data);
	mlx_scroll_hook(data.mlx, scroll_hook, &data);
	mlx_resize_hook(data.mlx, resize_hook, &data);
	mlx_close_hook(data.mlx, cleanup, &data);
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}
