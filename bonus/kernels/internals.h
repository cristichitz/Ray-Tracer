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

typedef float3		t_cl_float3;

# else

typedef cl_float3	t_cl_float3;

# endif

typedef struct s_interval
{
	float		min;
	float		max;
	float		size;
}				t_interval;

typedef struct s_ray
{
	t_cl_float3	origin;
	t_cl_float3	dir;
}				t_ray;

typedef struct s_material
{
	t_cl_float3	albedo;
	int			type;
	int			dynamic;
	float		density;
	float		restitution;
	float		friction;
}				t_material;

typedef struct s_hit_record
{
	t_cl_float3	p;
	t_cl_float3	normal;
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
	t_cl_float3	throughput;
	t_cl_float3	out;
	bool		count_emission;
}				t_path;

typedef struct s_scat
{
	t_cl_float3	attenuation;
	t_ray		scattered;
}				t_scat;

typedef struct s_trace
{
	t_ray			ray;
	t_hit_record	rec;
	float			tmin;
	float			closest;
}				t_trace;

#endif
