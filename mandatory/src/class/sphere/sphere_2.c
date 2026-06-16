/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 11:50:11 by timurray          #+#    #+#             */
/*   Updated: 2026/06/16 11:50:44 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sphere.h"

void	material_sphere(void *base)
{
	t_sphere	*self;

	self = (t_sphere *)base;
	if (self->mat.type == MAT_LAMBERTIAN)
		self->mat = init_metal(self->mat.tex.albedo);
	else if (self->mat.type == MAT_METAL)
		self->mat = init_lambertian(self->mat.tex.albedo);
}
