/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 17:51:08 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/13 18:07:20 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "box.h"

bool	hit_box(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_box			*self;
	t_hit_record	tmp;
	bool			hit;
	float			closest;
	int				i;

	self = (t_box *)base;
	tmp.set_face_normal = ft_set_face_normal;
	hit = false;
	closest = t.max;
	i = 0;
	while (i < 6)
	{
		if (self->sides[i]->base.hit(self->sides[i], ray, interval_init(t.min,
					closest), &tmp))
		{
			hit = true;
			closest = tmp.t;
			*rec = tmp;
		}
		i++;
	}
	return (hit);
}

static void	destroy_box(void *base)
{
	t_box	*self;
	int		i;

	self = (t_box *)base;
	i = 0;
	while (i < 6)
	{
		free(self->sides[i]);
		i++;
	}
}

static void	assemble_boxi(t_box *boxi, t_vec3 min, t_vec3 max, t_material mat)
{
	t_vec3	dx;
	t_vec3	dy;
	t_vec3	dz;

	dx = make_vec(max.x - min.x, 0.0f, 0.0f);
	dy = make_vec(0.0f, max.y - min.y, 0.0f);
	dz = make_vec(0.0f, 0.0f, max.z - min.z);
	boxi->sides[0] = make_quad(make_vec(min.x, min.y, max.z), dx, dy,
			mat);
	boxi->sides[1] = make_quad(make_vec(max.x, min.y, max.z), scale(dz, -1), dy,
			mat);
	boxi->sides[2] = make_quad(make_vec(max.x, min.y, min.z), scale(dx, -1), dy,
			mat);
	boxi->sides[3] = make_quad(make_vec(min.x, min.y, min.z), dz, dy,
			mat);
	boxi->sides[4] = make_quad(make_vec(min.x, max.y, max.z), dx, scale(dz, -1),
			mat);
	boxi->sides[5] = make_quad(make_vec(min.x, min.y, min.z), dx, dz,
			mat);
	boxi->base.hit = hit_box;
	boxi->base.destroy = destroy_box;
	boxi->base.resize = NULL;
	boxi->base.rotate = NULL;
	boxi->base.material = NULL;
}

t_box	*make_box(t_vec3 a, t_vec3 b, t_material mat)
{
	t_box	*boxi;
	t_vec3	min;
	t_vec3	max;
	int		i;

	boxi = malloc(sizeof(t_box));
	if (!boxi)
		return (NULL);
	min = make_vec(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
	max = make_vec(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));
	assemble_boxi(boxi, min, max, mat);
	i = 0;
	while (i < 6)
	{
		if (!boxi->sides[i])
		{
			destroy_box(boxi);
			free(boxi);
			return (NULL);
		}
		i++;
	}
	return (boxi);
}
