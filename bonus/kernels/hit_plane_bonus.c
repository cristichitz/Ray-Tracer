/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_plane_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:24 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:05:43 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

bool	hit_plane(t_object self, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
	float	denom;
	float	t;

	denom = dot(self.normal, ray.dir);
	if (fabs(denom) < 1e-8f)
		return (false);
	t = (self.d - dot(self.normal, ray.origin)) / denom;
	if (!interval_contains(&ray_t, t))
		return (false);
	rec->p = ray_at(&ray, t);
	rec->t = t;
	rec->mat = self.material;
	ft_set_face_normal(rec, ray, self.normal);
	return (true);
}
