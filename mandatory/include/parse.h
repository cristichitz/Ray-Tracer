/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 15:18:29 by timurray          #+#    #+#             */
/*   Updated: 2026/06/14 12:46:31 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
# define PARSE_H

# include "libft.h"
# include "rt_cpu.h"
# include <fcntl.h>
# include <float.h>
# include <limits.h>
# include <stdlib.h>

# define COORD_MAX 2000.0
# define COORD_MIN -2000.0

# define LIGHT_QUAD_SIZE 20.0f
# define LIGHT_QUAD_GAIN 4.0f

int		parse_input(t_data *data, int ac, char **av);

int		process_scene(t_data *data, int fd);
int		valid_scene(t_data *data);
int		add_to_world(t_data *data, void *object);

int		process_line(t_data *data, char *line);
int		process_type(t_data *data, char **params);

size_t	split_len(char **split);
int		split_count(char **params, size_t expected);
int		force_normalised(t_vec3 *v);

int		get_float(float *num, char *param, float min, float max);
int		get_int(int *num, char *param, int min, int max);
int		get_pt(float *num, char *param);
int		get_uvec_pt(float *num, char *param);
int		get_normed_float(float *num, char *param, float min, float max);

int		valid_filename(char *filename, const char *ext);
int		set_pts(t_vec3 *pt, char *params, int (*f)(float *n, char *p));

int		set_colour(t_vec3 *colour, char *params);
int		set_material(t_material *mat, char *params);
int		set_radius(float *num, char *param);
int		set_height(float *num, char *param);
int		set_fov(int *num, char *param);

int		set_ambient_light(t_data *data, char **params);
int		add_light_quad(t_data *data, t_light light);
int		set_brightness(float *fnum, char *param);

int		set_cam(t_data *data, char **params);
int		set_light(t_data *data, char **params);
int		set_sphere(t_data *data, char **params);
int		set_cylinder(t_data *data, char **params);
int		set_plane(t_data *data, char **params);
int		set_quad(t_data *data, char **params);
int		set_box(t_data *data, char **params);

#endif