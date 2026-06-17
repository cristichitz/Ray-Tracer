/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_types_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:34:58 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_TYPES_BONUS_H
# define RT_TYPES_BONUS_H

# include "CL/cl.h"
# include "MLX42/MLX42.h"
# include "init_kernel.h"

# define BOX_FACES 6
# define PHYS_DT 0.030f
# define PHYS_SUBSTEPS 6
# define PHYS_GRAVITY -70.0f
# define PHYS_RESTITUTION 0.22f
# define PHYS_FRICTION 0.45f
# define PHYS_LIN_DAMP 0.995f
# define PHYS_ANG_DAMP 0.985f
# define PHYS_BAUMGARTE 0.30f
# define PHYS_SLOP 0.01f
# define PHYS_SLEEP_VEL 0.8f
# define PHYS_MAX_BODIES 128
# define PHYS_MAX_COLLIDERS 32
# define PHYS_LAUNCH_SPEED 80.0f
# define PHYS_CLICK_IMPULSE 6000.0f

typedef struct s_gpu
{
	cl_context			context;
	cl_command_queue	queue;
	cl_program			program;
	cl_kernel			kernel;
	cl_mem				buffer;
	cl_mem				object_buffer;
	cl_mem				accum_buffer;
	cl_mem				node_buffer;
	cl_mem				prim_buffer;
}						t_gpu;

typedef struct s_bvh
{
	t_bvh_node			*nodes;
	int					nnodes;
	int					*prim;
	int					nprim;
	int					plane_count;
}						t_bvh;

// Unit quaternion (w + xi + yj + zk); carries a rigid body's orientation.
typedef struct s_quat
{
	float				w;
	float				x;
	float				y;
	float				z;
}						t_quat;

typedef struct s_rbody
{
	int					obj_first;
	int					obj_count;
	int					shape;
	cl_float3			pos;
	cl_float3			vel;
	t_quat				orient;
	cl_float3			omega;
	float				inv_mass;
	float				inv_i;
	float				half;
	float				restitution;
	float				friction;
	int					sleeping;
	cl_float3			loc_c[BOX_FACES];
	cl_float3			loc_u[BOX_FACES];
	cl_float3			loc_v[BOX_FACES];
}						t_rbody;

typedef struct s_collider
{
	cl_float3			n;
	float				d;
	float				restitution;
	float				friction;
}						t_collider;

typedef struct s_physics
{
	t_rbody				bodies[PHYS_MAX_BODIES];
	int					count;
	t_collider			colliders[PHYS_MAX_COLLIDERS];
	int					collider_count;
	int					running;
	int					autostart;
	int					settle;
	float				floor_y;
}						t_physics;

typedef struct s_cyl_size
{
	float				radius;
	float				height;
}						t_cyl_size;

typedef struct s_contact
{
	cl_float3			n;
	cl_float3			ra;
	cl_float3			rb;
	float				jn;
}						t_contact;

typedef struct s_data
{
	void				*mlx;
	mlx_image_t			*img;
	t_physics			phys;
	cl_float3			cam_center;
	cl_float3			cam_dir;
	float				cam_yaw;
	float				cam_pitch;
	float				cam_fov;
	t_gpu				gpu;
	cl_int				err;
	t_image				frame;
	t_object			*objects;
	uint32_t			obj_count;
	t_bvh				bvh;
	int					frame_index;
	int					scene_dirty;
	int					render_mode;
	int					render_started;
	int					render_frame_no;
}						t_data;

#endif
