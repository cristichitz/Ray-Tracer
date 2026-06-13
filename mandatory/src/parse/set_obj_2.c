/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_obj_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:25:20 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:30:49 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"
#include "rt_cpu.h"

int	add_light_quad(t_data *data, t_light light)
{
	t_vec3		u;
	t_vec3		v;
	t_vec3		corner;
	t_material	difflight;

	u = make_vec(LIGHT_QUAD_SIZE, 0.0f, 0.0f);
	v = make_vec(0.0f, 0.0f, LIGHT_QUAD_SIZE);
	corner = sub(light.center, scale(add(u, v), 0.5f));
	difflight = init_diffuse_light(scale(light.colour, light.brightness
				* LIGHT_QUAD_GAIN));
	data->light_i = data->world.objects->len;
	return (add_to_world(data, make_quad(corner, u, v, difflight)));
}
