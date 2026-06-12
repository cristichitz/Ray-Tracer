/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collide_sat_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:56 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:57 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Separating Axis Theorem overlap test for two oriented boxes: 15 candidate
** axes (3 + 3 face normals, 9 edge cross products). The smallest overlap and
** its axis become the contact normal and penetration depth.
*/

/*
** Test candidate axis l. Returns 0 if it separates the boxes; otherwise keeps
** the smallest overlap so far in mp[0] and its (normalized) axis in mp[1..3].
*/
static int	sat_axis(t_rbody *a, t_rbody *b, cl_float3 l, float *mp)
{
	cl_float3	d;
	float		proj[3];

	if (dot(l, l) < 1e-6f)
		return (1);
	l = norm(l);
	d = sub(b->pos, a->pos);
	proj[0] = a->half * (fabsf(dot(box_axis(a, 0), l))
			+ fabsf(dot(box_axis(a, 1), l)) + fabsf(dot(box_axis(a, 2), l)));
	proj[1] = b->half * (fabsf(dot(box_axis(b, 0), l))
			+ fabsf(dot(box_axis(b, 1), l)) + fabsf(dot(box_axis(b, 2), l)));
	proj[2] = proj[0] + proj[1] - fabsf(dot(d, l));
	if (proj[2] < 0.0f)
		return (0);
	if (proj[2] < mp[0])
	{
		mp[0] = proj[2];
		mp[1] = l.x;
		mp[2] = l.y;
		mp[3] = l.z;
	}
	return (1);
}

static int	face_axes(t_rbody *a, t_rbody *b, float *mp)
{
	int	i;

	i = -1;
	while (++i < 3)
	{
		if (!sat_axis(a, b, box_axis(a, i), mp)
			|| !sat_axis(a, b, box_axis(b, i), mp))
			return (0);
	}
	return (1);
}

static int	edge_axes(t_rbody *a, t_rbody *b, float *mp)
{
	int	i;
	int	j;

	i = -1;
	while (++i < 3)
	{
		j = -1;
		while (++j < 3)
		{
			if (!sat_axis(a, b, cross(box_axis(a, i), box_axis(b, j)), mp))
				return (0);
		}
	}
	return (1);
}

/* Full 15-axis SAT. Fills *n (a->b) and *pen on overlap; 0 when apart. */
int	sat_overlap(t_rbody *a, t_rbody *b, cl_float3 *n, float *pen)
{
	float	mp[4];

	mp[0] = 1e30f;
	mp[1] = 0.0f;
	mp[2] = 1.0f;
	mp[3] = 0.0f;
	if (!face_axes(a, b, mp) || !edge_axes(a, b, mp))
		return (0);
	*n = make_float3(mp[1], mp[2], mp[3]);
	if (dot(sub(b->pos, a->pos), *n) < 0.0f)
		*n = scale(*n, -1.0f);
	*pen = mp[0];
	return (1);
}
