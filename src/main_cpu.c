#include "parse.h"
#include "rt_cpu.h"
#include "bench.h"
#include <string.h>
#include <stdlib.h>

float	random_float(float min, float max)
{
	// random is a number between 0 and 1
	return (min + (max - min) * ((rand() / ((double)RAND_MAX + 1))));
}

// Constants
float	deg_to_rad(float degrees)
{
	return (degrees * M_PI / 180.0f);
}

static void	update_viewport(t_data *data)
{
	t_vec3	upper_left_cornerr;
	t_vec3	w;

	data->origin = make_vec(data->cam.center.x, data->cam.center.y,
			data->cam.center.z);
	
	w = scale(data->cam.dir, -1.0f);
	upper_left_cornerr = sub(data->origin, scale(data->horizontal, 0.5f));
	upper_left_cornerr = sub(upper_left_cornerr, scale(data->vertical, 0.5f));
	upper_left_cornerr = sub(upper_left_cornerr, scale(w, data->focal_length));
	data->pixel00_loc = add(upper_left_cornerr, scale(add(data->px_w,
					data->px_h), 0.5f));
}

void	game_loop(void *param)
{
	t_data	*data;
	float	speed;

	data = (t_data *)param;
	speed = 1.0f;
	if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(data->mlx);
	if (mlx_is_key_down(data->mlx, MLX_KEY_W))
		data->cam.center.z -= speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_S))
		data->cam.center.z += speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_A))
		data->cam.center.x -= speed;
	if (mlx_is_key_down(data->mlx, MLX_KEY_D))
		data->cam.center.x += speed;
	update_viewport(data);
	render_frame(data);
}

void	initialize(t_data *data)
{
	t_vec3 vup;
	t_vec3 w, u, v;


	data->width = WIDTH;
	data->height = (int)(data->width / (16.0 / 9.0));
	data->height = (data->height < 1) ? 1 : data->height;
	data->aspect_ratio = (float)data->width / (float)data->height;
	data->viewport_h = 2.0f * tanf(deg_to_rad((float)data->cam.fov) / 2.0f);
	data->viewport_w = data->aspect_ratio * data->viewport_h;
	data->focal_length = 1.0f;
	// New. For Antialising
	data->samples_per_pixel = 50;
	data->max_depth = 3;
	data->pixel_samples_scale = 1.0f / data->samples_per_pixel;
	
	vup = make_vec(0.0f, 1.0f, 0.0f);
	w = scale(data->cam.dir, -1.0f);

	if (fabs(w.x) < 1e-5 && fabs(w.z) < 1e-5)
        vup = make_vec(0.0f, 0.0f, 1.0f);

	u = norm(cross(vup, w));
	v = cross(w, u);

	data->horizontal = scale(u, data->viewport_w);
	data->vertical = scale(v, -data->viewport_h);

	data->px_w = scale(data->horizontal, (float)1 / (float)data->width);
	data->px_h = scale(data->vertical, (float)1 / (float)data->height);
	update_viewport(data);
}

void	make_cornell_box(t_hittable_list *world)
{
	t_material  red = init_lambertian(make_vec(0.65f, 0.05f, 0.05f));
	t_material  white = init_lambertian(make_vec(0.73f, 0.73f, 0.73f));
	t_material  green = init_lambertian(make_vec(0.12f, 0.45f, 0.15f));
	t_material  difflight = init_diffuse_light(make_vec(15.0f, 15.0f, 15.0f));

	world->add(world, make_quad(make_vec(555, 0, 0), make_vec(0, 555, 0), make_vec(0, 0, 555), green));
	world->add(world, make_quad(make_vec(0, 0, 0), make_vec(0, 555, 0), make_vec(0, 0, 555), red));
	world->add(world, make_quad(make_vec(343, 554, 332), make_vec(-130, 0, 0), make_vec(0, 0, -105), difflight));
	world->add(world, make_quad(make_vec(0, 0, 0), make_vec(555, 0, 0), make_vec(0, 0, 555), white));
	world->add(world, make_quad(make_vec(555, 555, 555), make_vec(-555, 0, 0), make_vec(0, 0, -555), white));
	world->add(world, make_quad(make_vec(0, 0, 555), make_vec(555, 0, 0), make_vec(0, 555, 0), white));
}

int	main(int ac, char **av)
{
	t_data			data;
	t_hittable_list	world;
	t_list			obj;

	int				frames;
	int				bench;
	char			*scene;
	char			*pav[2];

	data.set_ambient_light = false;
	data.set_cam = false;
	data.set_light = false;
	data.headless = false;
	data.fb = NULL;
	world.objects = &obj;
	if (init_world(&world))
		return (EXIT_FAILURE);
	data.world = world;
	frames = 30;
	bench = 0;
	scene = NULL;
	for (int i = 1; i < ac; i++)
	{
		if (!strcmp(av[i], "--bench"))
		{
			bench = 1;
			if (i + 1 < ac && av[i + 1][0] >= '0' && av[i + 1][0] <= '9')
				frames = atoi(av[++i]);
		}
		else if (!scene)
			scene = av[i];
	}
	if (!scene)
	{
		print_error("No scene file given.");
		return (0);
	}
	pav[0] = av[0];
	pav[1] = scene;
	if (!parse_input(&data, 2, pav))
	{
		//TODO: free memory
		return (EXIT_FAILURE);
	}

	// make_cornell_box(&world);
	initialize(&data);
	if (bench)
		return (run_benchmark(&data, frames));
	// Init MlX42
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
	mlx_loop(data.mlx);
	mlx_terminate(data.mlx);
	return (EXIT_SUCCESS);
}


