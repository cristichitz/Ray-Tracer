/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_world_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:14 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:40:50 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"

/* C  x,y,z  nx,ny,nz  fov */
int	set_cam(t_data *data, char **p)
{
	if (split_len(p) != 4)
		return (parse_err("C: expected 'C position orientation fov'."));
	if (!set_vec3(&data->cam_center, p[1]) || !set_vec3(&data->cam_dir, p[2]))
		return (0);
	data->cam_dir = norm(data->cam_dir);
	data->cam_fov = ft_strtof(p[3], NULL);
	return (1);
}

/* A  ratio  r,g,b   -> flat background colour used when a ray escapes */
int	set_ambient(t_data *data, char **p)
{
	cl_float3	color;
	float		ratio;

	if (split_len(p) != 3)
		return (parse_err("A: expected 'A ratio colour'."));
	ratio = ft_strtof(p[1], NULL);
	if (!set_color(&color, p[2]))
		return (0);
	data->frame.background = scale(color, ratio);
	return (1);
}

// L  x,y,z  brightness  r,g,b   (panel half-size = LIGHT_RADIUS)
int	set_light(t_data *data, char **p)
{
	cl_float3	center;
	cl_float3	color;
	float		brightness;
	t_material	emit;

	if (split_len(p) != 4)
		return (parse_err("L: expected 'L position brightness colour'."));
	if (!set_vec3(&center, p[1]) || !set_color(&color, p[3]))
		return (0);
	brightness = ft_strtof(p[2], NULL);
	emit = material_init(scale(color, brightness * LIGHT_GAIN), 2);
	center = sub(center, make_float3(LIGHT_RADIUS, 0.0f, LIGHT_RADIUS));
	return (add_object(data, make_obj_quad(center,
				make_float3(2.0f * LIGHT_RADIUS, 0.0f, 0.0f),
				make_float3(0.0f, 0.0f, 2.0f * LIGHT_RADIUS), emit)));
}
