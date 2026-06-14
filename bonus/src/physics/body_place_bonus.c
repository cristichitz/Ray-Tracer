/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   body_place_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 16:00:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Push one body's simulated pose back into the geometry the GPU renders. A
** sphere just moves its center; a box rebuilds each of its BOX_FACES quads
** from the cached local faces rotated by the body orientation and translated
** to the body center (so a tumbling box renders correctly).
*/
void	body_place(t_data *data, t_rbody *b)
{
	t_object	*o;
	cl_float3	n;
	int			f;

	if (b->shape == 1)
	{
		data->objects[b->obj_first].center = b->pos;
		return ;
	}
	f = 0;
	while (f < BOX_FACES)
	{
		o = &data->objects[b->obj_first + f];
		o->center = add(b->pos, quat_apply(b->orient, b->loc_c[f]));
		o->u = quat_apply(b->orient, b->loc_u[f]);
		o->v = quat_apply(b->orient, b->loc_v[f]);
		n = cross(o->u, o->v);
		o->normal = norm(n);
		o->d = dot(o->normal, o->center);
		o->w = divide(n, dot(n, n));
		f++;
	}
}
