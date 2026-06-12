/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_explode_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:30 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:30 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** The R-key cinematic, in three phases (t_rubik.explode_phase):
**   EXP_OUT : every cubie flies straight out from the solved cube.
**   EXP_SPIN: the cube hangs fully exploded while the camera orbits 720 deg
**             with an ease-in-out (slow, fast, slow) "bullet time" feel.
**   EXP_IN  : the cubies are sucked back in one by one, accelerating like a
**             gravity pull, but landing in a *scrambled* arrangement instead
**             of the solved one. The cube then immediately solves itself.
**
** Each cubie keeps the same 6 quads in data->objects; we rebuild their world
** geometry every frame from one canonical local box (loc_*) plus a per-cubie
** center and orientation (a quaternion), so the scramble lands with correct
** sticker orientations and the normal move engine can solve it cleanly.
*/

/* Capture the solved cube as the canonical box + per-cubie solved homes. */
static void	capture_cube(t_rubik *r, t_object *objs)
{
	cl_float3	c0;
	int			c;
	int			f;

	c0 = add(r->center, scale(make_float3(r->cubies[0].pos[0],
					r->cubies[0].pos[1], r->cubies[0].pos[2]), RUBIK_STEP));
	f = 0;
	while (f < FACES)
	{
		r->loc_c[f] = sub(objs[r->cubies[0].obj + f].center, c0);
		r->loc_u[f] = objs[r->cubies[0].obj + f].u;
		r->loc_v[f] = objs[r->cubies[0].obj + f].v;
		f++;
	}
	c = 0;
	while (c < CUBIES)
	{
		r->home_solved[c] = add(r->center, scale(make_float3(
						r->cubies[c].pos[0], r->cubies[c].pos[1],
						r->cubies[c].pos[2]), RUBIK_STEP));
		r->expl_dir[c] = norm(make_float3(r->cubies[c].pos[0],
					r->cubies[c].pos[1], r->cubies[c].pos[2]));
		c++;
	}
}

/*
** Kick off the cinematic, but only from a settled, solved cube (so the
** captured geometry is clean and the auto-solve afterwards is exact).
*/
void	start_explode(t_data *data)
{
	t_rubik		*r;
	cl_float3	rel;

	r = &data->rubik;
	if (r->explode_active || r->active || r->q_count != 0 || r->h_count != 0
		|| data->phys.running)
		return ;
	capture_cube(r, data->objects);
	plan_scramble(r);
	rel = sub(data->cam_center, r->center);
	r->cam_base_dist = sqrtf(dot(rel, rel));
	r->cam_base_dir = norm(rel);
	r->spin_angle = 0.0f;
	r->explode_phase = EXP_OUT;
	r->explode_frame = 0;
	r->explode_active = 1;
}

float	smoothstep(float t)
{
	t = fmaxf(0.0f, fminf(1.0f, t));
	return (t * t * (3.0f - 2.0f * t));
}

/*
** Place the camera: orbit `spin` radians around Y from its launch direction
** and back it off by `spread` so the whole exploded cube stays framed.
*/
void	explode_camera(t_data *data, float spread, float spin)
{
	t_rubik		*r;
	cl_float3	rel;

	r = &data->rubik;
	rel = r->cam_base_dir;
	rot_vec_axis(&rel, 1, cosf(spin), sinf(spin));
	rel = scale(rel, r->cam_base_dist + spread * EXPLODE_CAM_PULL);
	data->cam_center = add(r->center, rel);
	data->cam_dir = norm(sub(r->center, data->cam_center));
	data->cam_yaw = atan2f(data->cam_dir.x, data->cam_dir.z);
	data->cam_pitch = asinf(fmaxf(-1.0f, fminf(1.0f, data->cam_dir.y)));
}

/*
** Keep the camera orbiting the (now reassembled) cube while it solves itself.
** Called from the move engine each solve frame; step_rubik clears
** orbit_active once the solve finishes.
*/
void	explode_orbit_solve(t_data *data)
{
	data->rubik.spin_angle += EXPLODE_ORBIT_RATE;
	explode_camera(data, 0.0f, data->rubik.spin_angle);
}
