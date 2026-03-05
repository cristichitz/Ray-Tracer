/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/03/05 13:13:12 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

bool hit_cylinder(void)
{
	//TODO: implement hit cyclinder;
	return (true);
}

t_cylinder *make_cylinder(t_cylinder *cylinder_data)
{
	t_cylinder *c;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	c->center = cylinder_data->center;
	c->radius = cylinder_data->radius;
	c->height = cylinder_data->height;
	// c->colour = cylinder_data->colour;
	// c->u_vec = cylinder_data->u_vec
	c->base.hit = hit_cylinder;
	return (c);
}