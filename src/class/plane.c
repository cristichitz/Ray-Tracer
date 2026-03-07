/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 17:05:37 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 19:37:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "plane.h"

bool hit_plane(void)
{
	//TODO: implement hit plane.
	return (true);
}

t_plane *make_plane(t_plane plane)
{
	t_plane *p;

	p = malloc(sizeof(t_plane));
	if(!p)
		return(NULL);
	*p = plane;
	// p->base.hit = hit_plane;
	return (p);
}
