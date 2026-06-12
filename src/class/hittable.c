/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:50:21 by timurray          #+#    #+#             */
/*   Updated: 2026/06/12 11:46:44 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

void	ft_set_face_normal(t_hit_record *self, t_ray r, t_vec3 outward_normal)
{
	self->front_face = dot(r.dir, outward_normal) < 0;
	if (self->front_face != true)
		outward_normal = scale(outward_normal, -1.0f);
	self->normal = outward_normal;
}

int	add_object(t_hittable_list *self, void *object)
{
	if (ft_vec_push(self->objects, object) != 1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

void	destroy_objects(t_hittable_list *self)
{
	size_t	i;
	void	*obj;

	i = 0;
	while (i < self->objects->len)
	{
		obj = ft_vec_get(self->objects, i);
		free(obj);
		i++;
	}
	ft_vec_free(self->objects);
}

bool	hit_objects(t_hittable_list *self, t_ray ray, t_interval t,
		t_hit_record *rec)
{
	t_hit_record	temp_rec;
	bool			hit_anything;
	float			closest_so_far;
	uint32_t		i;
	t_hittable		*object;

	temp_rec.set_face_normal = ft_set_face_normal;
	hit_anything = false;
	closest_so_far = t.max;
	i = 0;
	while (i < self->objects->len)
	{
		object = ft_vec_get(self->objects, i);
		if (object->hit(object, ray, interval_init(t.min, closest_so_far),
				&temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
		i++;
	}
	return (hit_anything);
}

int	init_world(t_hittable_list *world)
{
	world->add = add_object;
	world->destroy = destroy_objects;
	world->hit = hit_objects;
	world->background = make_vec(0.0f, 0.0f, 0.0f);
	world->ambient = make_vec(0.0f, 0.0f, 0.0f);
	return (EXIT_SUCCESS);
}
