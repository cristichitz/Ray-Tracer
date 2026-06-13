/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_cpu.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:07:50 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 19:39:03 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_CPU_H
# define RT_CPU_H

# include "MLX42/MLX42.h"
# include "cam.h"
# include "cylinder.h"
# include "libft.h"
# include "light.h"
# include "plane.h"
# include "quad.h"
# include "sphere.h"
# include "box.h"

# define WIDTH 640

typedef enum e_quality
{
	LOW,
	HIGH
}								t_quality;

typedef enum e_render_mode
{
	RENDER_PATH_TRACE,
	RENDER_DIRECT
}								t_render_mode;

typedef t_vec					t_list;

typedef struct s_hittable_list	t_hittable_list;

typedef bool					(*t_world_hit)(t_hittable_list *self, t_ray r,
				t_interval t, t_hit_record *rec);

typedef struct s_hittable_list
{
	t_list						*objects;
	int							(*add)(t_hittable_list *self, void *object);
	void						(*destroy)(t_hittable_list *self);
	t_world_hit					hit;
	t_vec3						background;
	t_vec3						ambient;
}								t_hittable_list;

typedef struct s_data
{
	void						*mlx;
	void						*img;

	uint32_t					*fb;
	bool						headless;

	uint32_t					width;
	uint32_t					height;

	uint32_t					max_depth;
	uint32_t					samples_per_pixel;
	float						pixel_samples_scale;

	t_cam						cam;
	t_ambient_light				ambient_light;
	t_light						light;

	float						aspect_ratio;
	float						viewport_w;
	float						viewport_h;
	float						focal_length;

	t_vec3						origin;
	t_vec3						pixel00_loc;

	t_vec3						horizontal;
	t_vec3						vertical;

	t_vec3						px_w;
	t_vec3						px_h;

	t_vec3						cam_forward;

	bool						set_cam;
	bool						set_ambient_light;
	bool						set_light;

	t_hittable_list				world;
	unsigned int				object_i;
	unsigned int				light_i;
	uint32_t					wait_frames;
	bool						render_check;
	t_render_mode				render_mode;
}								t_data;

int								init_world(t_hittable_list *world);

// MLX Loop
int								render_frame(t_data *data);
int								render_frame_direct(t_data *data);
t_ray							get_ray(t_data *data, uint32_t x, uint32_t y);
void							write_color(t_data *data, uint32_t x,
									uint32_t y, t_vec3 color);
// int								close_app(t_data *data);
// int								key_hook(int keycode, t_data *data);

void							init_viewport(t_data *data);
void							camera_setup(t_data *data);
void							update_viewport(t_data *data);
bool							move_cam(t_data *data, float *speed);
bool							rotate_cam(t_data *data, float *rot_speed);

void							object_selector(mlx_key_data_t keydata,
									void *param);
bool							move_object(t_data *data, float *speed);
bool							resize_object(t_data *data, float *scalar);
bool							rotate_object(t_data *data,
									float *rotation_speed);

bool							is_emissive(t_hit_record *rec);
t_vec3							sample_square(void);
float							clampf(float val, float min, float max);
void							set_quality(t_data *data, t_quality quality);
void							set_render_mode(t_data *data,
									t_render_mode mode);

void							print_error(char *msg);
int								return_print_error(char *msg, int err);

#endif
