/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_physics_bonus.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_PHYSICS_BONUS_H
# define RT_PHYSICS_BONUS_H

# include "rt_types_bonus.h"

// Quaternion helpers (quat_*_bonus.c)
t_quat					quat_identity(void);
t_quat					quat_axis(int axis, float angle);
t_quat					quat_from_axis(cl_float3 axis, float angle);
t_quat					quat_mul(t_quat a, t_quat b);
cl_float3				quat_apply(t_quat q, cl_float3 v);
t_quat					quat_normalize(t_quat q);
t_quat					quat_integrate(t_quat q, cl_float3 w, float dt);

// Rigid bodies: registration during build + per-frame posing (body_bonus.c)
void					add_body(t_data *data, int first, int count,
							t_material mat);
void					body_place(t_data *data, t_rbody *b);

// Rigid-body physics (src/physics/*.c)
void					physics_init(t_data *data);
void					physics_step(t_data *data);
void					collide_world(t_physics *ph, t_rbody *b);
void					collide_plane(t_rbody *b, t_collider c);
void					collide_ground(t_rbody *b, float floor_y);
void					collide_pair(t_rbody *a, t_rbody *b);
void					contact_impulse(t_rbody *a, t_rbody *b, cl_float3 n,
							cl_float3 cp);
void					collide_ball(t_rbody *a, t_rbody *b);
void					ball_ball(t_rbody *a, t_rbody *b);
void					collide_ball_ground(t_rbody *b, float floor_y);
cl_float3				box_axis(t_rbody *b, int i);
cl_float3				box_vertex(t_rbody *b, int i);
cl_float3				contact_point(t_rbody *a, t_rbody *b);
int						sat_overlap(t_rbody *a, t_rbody *b, cl_float3 *n,
							float *pen);

// Modular physics input: impulses + ray picking (physics_input_bonus.c)
void					apply_impulse(t_rbody *b, cl_float3 imp,
							cl_float3 point);
int						pick_body(t_data *data, t_ray ray);
t_ray					ray_from_screen(t_data *data, float sx, float sy);
void					shove_forward(t_data *data);

#endif
