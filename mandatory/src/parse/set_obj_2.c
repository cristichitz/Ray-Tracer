/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_obj_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:25:20 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 20:59:48 by cdohanic         ###   ########.fr       */
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

int	set_quad(t_data *data, char **params)
{
	t_vec3	q;
	t_vec3	u;
	t_vec3	v;
	t_vec3	colour;

	if (!split_count(params, 5))
		return (0);
	if (!set_pts(&q, params[1], get_pt))
		return (0);
	if (!set_pts(&u, params[2], get_pt))
		return (0);
	if (!set_pts(&v, params[3], get_pt))
		return (0);
	if (!set_colour(&colour, params[4]))
		return (0);
	return (add_to_world(data, make_quad(q, u, v, init_lambertian(colour))));
}

int	set_box(t_data *data, char **params)
{
	t_vec3	a;
	t_vec3	b;
	t_vec3	colour;

	if (!split_count(params, 4))
		return (0);
	if (!set_pts(&a, params[1], get_pt))
		return (0);
	if (!set_pts(&b, params[2], get_pt))
		return (0);
	if (!set_colour(&colour, params[3]))
		return (0);
	return (add_to_world(data, make_box(a, b, init_lambertian(colour))));
}
