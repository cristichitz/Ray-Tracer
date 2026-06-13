/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:56:32 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 19:30:58 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

t_vec3	color_value(t_texture *self, float u, float v, t_vec3 p)
{
	(void)self;
	(void)u;
	(void)v;
	(void)p;
	return (self->albedo);
}

t_texture	init_solid_color(t_vec3 col)
{
	t_texture	tex;

	tex.albedo = col;
	tex.value = color_value;
	return (tex);
}
