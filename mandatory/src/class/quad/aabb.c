/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 20:55:24 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 20:56:08 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "quad.h"
#include "quaternion.h"

void	pad_to_minimums(t_aabb *p)
{
	float	delta;

	delta = 0.0001;
	if (p->x.size < delta)
		p->x = p->x.expand(&(p->x), delta);
	if (p->y.size < delta)
		p->y = p->y.expand(&(p->y), delta);
	if (p->z.size < delta)
		p->z = p->z.expand(&(p->z), delta);
}

t_aabb	make_aabb_from_aabbs(t_aabb box0, t_aabb box1)
{
	t_aabb	merged;

	merged.x = interval_init(fmin(box0.x.min, box1.x.min), fmax(box0.x.max,
				box1.x.max));
	merged.y = interval_init(fmin(box0.y.min, box1.y.min), fmax(box0.y.max,
				box1.y.max));
	merged.z = interval_init(fmin(box0.z.min, box1.z.min), fmax(box0.z.max,
				box1.z.max));
	return (merged);
}

t_aabb	make_aabb(t_vec3 a, t_vec3 b)
{
	t_aabb	ab;

	ab.x = interval_init(fmin(a.x, b.x), fmax(a.x, b.x));
	ab.y = interval_init(fmin(a.y, b.y), fmax(a.y, b.y));
	ab.z = interval_init(fmin(a.z, b.z), fmax(a.z, b.z));
	pad_to_minimums(&ab);
	return (ab);
}
