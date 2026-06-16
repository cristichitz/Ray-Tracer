/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_obj_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:19 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:20 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"

/* sp  x,y,z  diameter  r,g,b  [material] */
int	set_sphere(t_data *data, char **p)
{
	cl_float3	center;
	cl_float3	color;
	t_material	mat;
	float		diameter;

	if (split_len(p) != 4 && split_len(p) != 5)
		return (parse_err("sp: expected 'sp center diameter colour [mat]'."));
	if (!set_vec3(&center, p[1]) || !set_color(&color, p[3]))
		return (0);
	diameter = ft_strtof(p[2], NULL);
	mat = material_named(mat_token(p, 4), color);
	if (!add_object(data, make_obj_sphere(center, diameter / 2.0f, mat)))
		return (0);
	if (mat.dynamic)
		add_body(data, (int)data->obj_count - 1, 1, mat);
	return (1);
}

/* pl  x,y,z  nx,ny,nz  r,g,b  [material] (planes are static colliders) */
int	set_plane(t_data *data, char **p)
{
	cl_float3	point;
	cl_float3	normal;
	cl_float3	color;

	if (split_len(p) != 4 && split_len(p) != 5)
		return (parse_err("pl: expected 'pl point normal colour [mat]'."));
	if (!set_vec3(&point, p[1]) || !set_vec3(&normal, p[2])
		|| !set_color(&color, p[3]))
		return (0);
	return (add_object(data, make_obj_plane(point, norm(normal),
				material_named(mat_token(p, 4), color))));
}

/* cy  x,y,z  nx,ny,nz  diameter  height  r,g,b  [material] */
int	set_cylinder(t_data *data, char **p)
{
	cl_float3	center;
	cl_float3	axis;
	cl_float3	color;
	t_cyl_size	size;

	if (split_len(p) != 6 && split_len(p) != 7)
		return (parse_err(
				"cy: expected 'cy center axis diameter height colour [mat]'."));
	if (!set_vec3(&center, p[1]) || !set_vec3(&axis, p[2])
		|| !set_color(&color, p[5]))
		return (0);
	size.radius = ft_strtof(p[3], NULL) / 2.0f;
	size.height = ft_strtof(p[4], NULL);
	return (add_object(data, make_obj_cylinder(center, norm(axis), size,
				material_named(mat_token(p, 6), color))));
}

/* bx  cx,cy,cz  sx,sy,sz  r,g,b  [material] : a box (6 quads) = one body */
int	set_box(t_data *data, char **p)
{
	cl_float3	center;
	cl_float3	size;
	cl_float3	color;
	t_material	mat;
	int			base;

	if (split_len(p) != 4 && split_len(p) != 5)
		return (parse_err("bx: expected 'bx center size colour [mat]'."));
	if (!set_vec3(&center, p[1]) || !set_vec3(&size, p[2])
		|| !set_color(&color, p[3]))
		return (0);
	mat = material_named(mat_token(p, 4), color);
	base = (int)data->obj_count;
	if (!make_box(data, sub(center, scale(size, 0.5f)),
			add(center, scale(size, 0.5f)), mat))
		return (0);
	if (mat.dynamic)
		add_body(data, base, BOX_FACES, mat);
	return (1);
}

/* qu  qx,qy,qz  ux,uy,uz  vx,vy,vz  r,g,b  [material] (great for area lights) */
int	set_quad(t_data *data, char **p)
{
	cl_float3	q;
	cl_float3	u;
	cl_float3	v;
	cl_float3	color;

	if (split_len(p) != 5 && split_len(p) != 6)
		return (parse_err("qu: expected 'qu q u_edge v_edge colour [mat]'."));
	if (!set_vec3(&q, p[1]) || !set_vec3(&u, p[2]) || !set_vec3(&v, p[3])
		|| !set_color(&color, p[4]))
		return (0);
	return (add_object(data, make_obj_quad(q, u, v,
				material_named(mat_token(p, 5), color))));
}
