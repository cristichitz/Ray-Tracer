/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_cpu.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:07:50 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 18:12:06 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_CPU_H
# define RT_CPU_H

# include "MLX42/MLX42.h"
# include "cam.h"
# include "cylinder.h"
# include "light.h"
# include "plane.h"
# include "sphere.h"
# include "types.h"

# define WIDTH 640
# ifndef M_PI
#  define M_PI 3.14159265358979323846
# endif

typedef enum e_quality
{
	LOW,
	HIGH
}								t_quality;

typedef t_vec					t_list;

typedef struct s_hittable_list	t_hittable_list;
typedef struct s_hittable_list
{
	t_list						*objects;
	int							(*add)(t_hittable_list *self, void *object);
	void						(*destroy)(t_hittable_list *self);
	bool						(*hit)(t_hittable_list *self, t_ray r,
								t_interval t, t_hit_record *rec);
}								t_hittable_list;

typedef struct s_data
{
	void						*mlx;
	void						*img;

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

	bool						set_cam;
	bool						set_ambient_light;
	bool						set_light;

	t_hittable_list				world;
	unsigned int				object_i;
	bool						render_check;
}								t_data;

int								init_world(t_hittable_list *world);

int								render_frame(t_data *data);
int								close_app(t_data *data);
int								key_hook(int keycode, t_data *data);

float							random_float(float min, float max);
float							deg_to_rad(float degrees);

bool							move_cam(t_data *data, float *speed);
bool							move_object(t_data *data, float *speed);
void							object_selector(mlx_key_data_t keydata,
									void *param);
bool							resize_object(t_data *data, float *scalar);

void							set_quality(t_data *data, t_quality quality);

#endif
