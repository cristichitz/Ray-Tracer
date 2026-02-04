#ifndef MINIRT_H
# define MINIRT_H

# include "../include/CL/cl.h" // or just <CL/cl.h> if you add the -I flag
# include "MLX42/MLX42.h"
# include "math.h"
# include "stdio.h"
# include "vec3.h"
# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_gpu
{
	cl_context			context;
	cl_command_queue	queue;
	cl_program			program;
	cl_kernel			kernel;
	cl_mem				buffer;
}						t_gpu;

typedef struct s_data
{
	mlx_t				*mlx;
	mlx_image_t			*img;

	int					width;
	int					height;

	t_gpu				gpu;

	// FPS
	struct timeval		last_time;
	int					frame_count;
	double delta_time; // Time taken for last frame

	// Camera pos
	float				cam_x;
	float				cam_y;
	float				cam_z;
}						t_data;

void					my_mlx_pixel_put(t_data *data, int x, int y, int color);

void					init_gpu(t_data *data, char *kernel_file);
void					render_frame(t_data *data);
void					clean_gpu(t_data *data);

void					game_loop(void *param);

#endif