/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 12:13:26 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"

bool hit_cylinder(void)
{
	//TODO: implement hit cyclinder;
	return (true);
}

t_cylinder *make_cylinder(t_cylinder cylinder)
{
	t_cylinder *c;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	// c->base.hit = hit_cylinder;
	return (c);
}