/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_bonus.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 20:14:55 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:45:52 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_BONUS_H
# define RT_BONUS_H

# include "cl_util_bonus.h"
# include "rt_types_bonus.h"
# include "rt_math_bonus.h"
# include "rt_physics_bonus.h"

# include <limits.h>
# include <math.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

# define BONUS_WIDTH 1920
# define BONUS_SPP 2
# define BONUS_MAX_DEPTH 2
# define ACCUM_MAX 512
# define LIGHT_RADIUS 5.0f
# define LIGHT_GAIN 20.0f
# define RENDER_SPP 1
# define RENDER_MAX_FRAMES 3000
# define RENDER_DT 0.008f

// GPU setup / teardown (init_gpu_bonus.c, cleanup_bonus.c)
cl_int					init_gpu(t_data *data);
void					render_frame(t_data *data);
void					game_loop(void *param);
void					clean_gpu(t_data *data);
void					cleanup(void *param);
cl_int					init_objects(t_data *data);
void					resize_gpu_buffers(t_data *data);

// Camera key controls (controls_bonus.c)
int						handle_rotation(t_data *data);
int						handle_movement(t_data *data, cl_float3 *step);

// Camera / view (view_bonus.c)
void					initialize(t_data *data);
void					update_view(t_data *data);

// Object + material builders (host side)
t_material				material_init(cl_float3 color, int type);
t_object				make_obj_sphere(cl_float3 center, float radius,
							t_material mat);
t_object				make_obj_plane(cl_float3 point, cl_float3 normal,
							t_material mat);
t_object				make_obj_quad(cl_float3 q, cl_float3 u, cl_float3 v,
							t_material mat);
t_object				make_obj_cylinder(cl_float3 center, cl_float3 axis,
							t_cyl_size size, t_material mat);

// Scene assembly (scene_bonus.c)
int						add_object(t_data *data, t_object obj);
void					bvh_build(t_data *data);
void					obj_bounds(t_object *o, cl_float3 *mn, cl_float3 *mx);
void					bvh_range_bounds(t_data *d, int first, int count,
							cl_float3 box[2]);
int						make_box(t_data *data, cl_float3 a, cl_float3 b,
							t_material mat);
void					make_cornell_box(t_data *data);

// Named "physics materials" (material_preset_bonus.c)
t_material				material_named(const char *name, cl_float3 color);

// Offline render-to-disk (render_out_bonus.c)
int						render_mode_on(int argc, char **argv);
void					render_loop(void *param);

// Mlx hooks (utils_bonus.c)
void					resize_hook(int32_t width, int32_t height, void *param);
void					key_hook(mlx_key_data_t key, void *param);
void					mouse_hook(mouse_key_t button, action_t action,
							modifier_key_t mods, void *param);

// Parsing (parse_*_bonus.c)
int						load_scene(t_data *data, int argc, char **argv);
int						parse_scene(t_data *data, char *file);
int						parse_err(char *msg);
size_t					split_len(char **s);
int						set_vec3(cl_float3 *v, char *s);
int						set_color(cl_float3 *c, char *s);
char					*mat_token(char **p, size_t base);
int						set_cam(t_data *data, char **p);
int						set_ambient(t_data *data, char **p);
int						set_light(t_data *data, char **p);
int						set_sphere(t_data *data, char **p);
int						set_plane(t_data *data, char **p);
int						set_cylinder(t_data *data, char **p);
int						set_box(t_data *data, char **p);
int						set_quad(t_data *data, char **p);

#endif
