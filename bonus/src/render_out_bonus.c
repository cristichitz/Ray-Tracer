/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_out_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:02 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:36:45 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"
#include <sys/stat.h>

/* True if --render appears anywhere on the command line. */
int	render_mode_on(int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (!ft_strcmp(argv[i], "--render"))
			return (1);
		i++;
	}
	return (0);
}

/* Write the current image as frames/frame_NNNNN.ppm (RGB, alpha dropped). */
static void	save_frame_ppm(t_data *data, int n)
{
	char	path[64];
	FILE	*f;
	uint8_t	*px;
	int		i;
	int		total;

	snprintf(path, sizeof(path), "frames/frame_%05d.ppm", n);
	f = fopen(path, "wb");
	if (!f)
	{
		perror("render: fopen");
		return ;
	}
	fprintf(f, "P6\n%d %d\n255\n", data->frame.width, data->frame.height);
	px = data->img->pixels;
	total = data->frame.width * data->frame.height;
	i = 0;
	while (i < total)
	{
		fwrite(&px[i * 4], 1, 3, f);
		i++;
	}
	fclose(f);
}

/* The shot is over once the simulation has run itself to rest. */
static int	animation_done(t_data *data)
{
	return (!data->phys.running);
}

static void	accumulate(t_data *data)
{
	int	s;

	s = 0;
	while (s < RENDER_SPP)
	{
		data->frame_index = s;
		render_frame(data);
		s++;
	}
}

void	render_loop(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	if (!data->render_started)
	{
		mkdir("frames", 0755);
		shove_forward(data);
		data->render_started = 1;
	}
	physics_step(data);
	update_view(data);
	data->scene_dirty = 1;
	accumulate(data);
	save_frame_ppm(data, data->render_frame_no);
	data->render_frame_no++;
	printf("render: wrote frame %d\n", data->render_frame_no - 1);
	if (animation_done(data) || data->render_frame_no >= RENDER_MAX_FRAMES)
	{
		printf("render: done, %d frames in frames/\n", data->render_frame_no);
		mlx_close_window(data->mlx);
	}
}
