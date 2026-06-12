/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:54 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:55 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <stdlib.h>
#include <string.h>

/*
** The grand finale: once the R-key cinematic has solved the cube, the cubies
** that spell out the "middle finger" (the z=0 face minus its two top corners)
** are frozen in place, and every other cubie drops away as a rigid body that
** collides with the floor, the standing finger and the other falling cubies.
*/

/*
** Finger = a single z=0 slice in a "_|_" shape: the center column (x==0, the
** raised finger) plus the bottom row (y==-1, the base). Five cubies.
*/
int	cubie_is_finger(t_rubik *r, int c)
{
	int	*p;

	p = r->cubies[c].pos;
	if (p[2] != 0)
		return (0);
	return (p[0] == 0 || p[1] == -1);
}

static float	frand(float lo, float hi)
{
	return (lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
}

/* One body: frozen when part of the finger, tumbling rigid body otherwise. */
static void	init_body(t_data *data, int c, float half)
{
	t_rubik	*r;
	t_rbody	*b;

	r = &data->rubik;
	b = &data->phys.bodies[c];
	memset(b, 0, sizeof(*b));
	b->cubie = c;
	b->half = half;
	b->orient = quat_identity();
	b->pos = add(r->center, scale(make_float3(r->cubies[c].pos[0],
					r->cubies[c].pos[1], r->cubies[c].pos[2]), RUBIK_STEP));
	if (cubie_is_finger(r, c))
	{
		b->sleeping = 1;
		return ;
	}
	b->inv_mass = 1.0f;
	b->inv_i = 1.5f / (half * half);
	b->vel = make_float3(frand(-1.5f, 1.5f), frand(-0.5f, 0.5f),
			frand(-1.5f, 1.5f));
	b->omega = make_float3(frand(-5.0f, 5.0f), frand(-5.0f, 5.0f),
			frand(-5.0f, 5.0f));
}

/* Turn every non-finger cubie into a falling rigid body; freeze the finger. */
void	physics_start(t_data *data)
{
	float	half;
	int		c;

	half = RUBIK_STEP * 0.46f;
	data->phys.floor_y = data->rubik.center.y - RUBIK_STEP - half;
	c = 0;
	while (c < CUBIES)
	{
		init_body(data, c, half);
		c++;
	}
	data->phys.count = CUBIES;
	data->phys.settle = 0;
	data->phys.rocket_frame = 0;
	data->phys.finger_done = 0;
	data->phys.running = 1;
}
