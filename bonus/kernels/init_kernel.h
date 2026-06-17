/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_kernel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:03:47 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 18:55:20 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_KERNEL_H
# define INIT_KERNEL_H

# include "internals.h"

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

typedef struct s_cyl_var
{
	cl_float3	w;
	float		a;
	float		half_b;
	float		c;
	float		dv;
	float		wv;
	float		discriminant;
	float		sqrtd;
	float		root;
	float		m;
}				t_cyl_var;

typedef struct s_cap
{
	cl_float3	center;
	cl_float3	normal;
	float		radius;
	t_material	mat;
}				t_cap;

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

# ifdef __OPENCL_VERSION__

/* interval_bonus.c */
bool			interval_contains(t_interval *self, float x);
bool			interval_surrounds(t_interval *self, float x);

/* ray_bonus.c */
float3			ray_at(t_ray *self, float t);
void			ft_set_face_normal(t_hit_record *self, t_ray r,
					float3 outward_normal);

/* hit_cylinder_bonus.c & hit_cyl_caps_bonus.c */
bool			hit_caps(__constant t_object *self, t_ray ray, t_interval ray_t,
					t_hit_record *rec);
bool			hit_cyl_body(__constant t_object *self, t_ray ray, t_interval t,
					t_hit_record *rec);

/* object intersection */
bool			hit_cylinder(__constant t_object *self, t_ray ray, t_interval t,
					t_hit_record *rec);
bool			hit_sphere(__constant t_object *self, t_ray ray, t_interval t,
					t_hit_record *rec);
bool			hit_plane(__constant t_object *self, t_ray ray,
					t_interval ray_t, t_hit_record *rec);
bool			hit_quad(__constant t_object *self, t_ray ray, t_interval ray_t,
					t_hit_record *rec);

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

/* objects_bonus.c */
bool			hit_objects(t_scene sc, t_ray ray, t_interval t,
					t_hit_record *rec);

/* color_bonus.c */
float3			get_ray_color(t_scene sc, t_ray ray, t_image *f, uint *seed);
# endif

#endif
