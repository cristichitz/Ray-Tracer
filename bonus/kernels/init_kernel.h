/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_kernel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:03:47 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:51:15 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_KERNEL_H
# define INIT_KERNEL_H

# include "internals.h"

# ifdef __OPENCL_VERSION__

typedef float3	cl_float3;
# endif

# define RAND_MAX 2147483647

# define OBJ_SPHERE 0
# define OBJ_PLANE 1
# define OBJ_CYLINDER 2
# define OBJ_QUAD 3

typedef struct s_object
{
	int			type;
	cl_float3	center;
	cl_float3	normal;
	cl_float3	u;
	cl_float3	v;
	cl_float3	w;
	float		radius;
	float		height;
	float		d;
	float		d_top;
	t_material	material;
}				t_object;

typedef struct s_bvh_node
{
	cl_float3	bmin;
	cl_float3	bmax;
	int			left;
	int			right;
	int			count;
	int			pad;
}				t_bvh_node;

typedef struct s_sphere_var
{
	float		a;
	float		h;
	float		c;
	float		discriminant;
	float		sqrtd;
	float		root;
}				t_sphere_var;

# ifdef __OPENCL_VERSION__

typedef struct s_scene
{
	__constant t_object			*objs;
	__global const t_bvh_node	*nodes;
	__global const int			*prim;
	int							count;
	int							nnodes;
	int							plane_first;
	int							plane_count;
}				t_scene;
# endif

typedef struct s_image
{
	int			width;
	int			height;

	float		aspect_ratio;
	float		viewport_height;
	float		viewport_width;
	float		focal_length;

	cl_float3	horizontal;
	cl_float3	vertical;

	int			samples_per_pixel;
	float		pixel_samples_scale;
	int			max_depth;

	cl_float3	origin;
	cl_float3	pixel00_loc;
	cl_float3	pixel_delta_u;
	cl_float3	pixel_delta_v;

	cl_float3	background;
}				t_image;

#endif
