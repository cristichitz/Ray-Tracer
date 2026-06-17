/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internals.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:04:58 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 18:25:05 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERNALS_H
# define INTERNALS_H

# ifdef __OPENCL_VERSION__

typedef float3	cl_float3;
# endif

typedef struct s_interval
{
	float		min;
	float		max;
	float		size;
}				t_interval;

typedef struct s_ray
{
	cl_float3	origin;
	cl_float3	dir;
}				t_ray;

typedef struct s_material
{
	cl_float3	albedo;
	int			type;
	int			dynamic;
	float		density;
	float		restitution;
	float		friction;
}				t_material;

typedef struct s_hit_record
{
	cl_float3	p;
	cl_float3	normal;
	float		t;
	float		u;
	float		v;
	bool		front_face;
	int			obj_type;
	t_material	mat;
}				t_hit_record;

typedef struct s_path
{
	t_ray		ray;
	cl_float3	throughput;
	cl_float3	out;
	bool		count_emission;
}				t_path;

typedef struct s_scat
{
	cl_float3	attenuation;
	t_ray		scattered;
}				t_scat;

typedef struct s_trace
{
	t_ray		ray;
	float		tmin;
	float		closest;
}				t_trace;

#endif
