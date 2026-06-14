/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_out_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:02 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:03 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <string.h>
#include <sys/stat.h>

/*
** Offline render-to-disk mode (run the bonus with --render).
**
** Interactive playback is driven by the keyboard (R) and the GPU has to keep
** up in real time, so quality is capped. For a final render we instead want
** to trade time for quality: play the whole cinematic automatically,
** accumulate many samples per frame, and write every frame to disk as a
** numbered image. A C program can't practically mux an MP4, but writing a
** frame sequence is trivial; assemble it afterwards with, e.g.:
**
**   ffmpeg -framerate 60 -i frames/frame_%05d.ppm -pix_fmt yuv420p out.mp4
**
** PPM (P6) is used because it needs no library (MLX42's bundled lodepng ships
** without its encoder). Files are big but ffmpeg compresses them away.
*/

/* True if --render appears anywhere on the command line. */
int	render_mode_on(int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (!strcmp(argv[i], "--render"))
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

/*
** One offline frame: advance the animation a single step, accumulate
** RENDER_SPP samples into a converged image, and write it out. The window
** still mirrors each frame as a live preview. Exits once the animation
** finishes.
*/
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
