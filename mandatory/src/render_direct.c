/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_direct.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:50:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/12 21:21:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

static bool	is_emissive(t_hit_record *rec)
{
	t_vec3	emission;

	emission = rec->mat.emitted(&rec->mat, 0, 0, rec->p);
	return (emission.x > 0.0f || emission.y > 0.0f || emission.z > 0.0f);
}

static bool	in_shadow(t_data *data, t_hit_record *rec)
{
	t_vec3			to_light;
	float			light_dist;
	t_ray			shadow_ray;
	t_hit_record	shadow_rec;

	to_light = sub(data->light.center, rec->p);
	light_dist = sqrtf(length_squared(to_light));
	shadow_ray = make_ray(rec->p, divide(to_light, light_dist));
	shadow_rec.set_face_normal = ft_set_face_normal;
	if (!data->world.hit(&data->world, shadow_ray,
			interval_init(0.001f, light_dist), &shadow_rec))
		return (false);
	if (is_emissive(&shadow_rec))
		return (false);
	return (true);
}

static t_vec3	compute_direct_light(t_data *data, t_hit_record *rec, t_ray cam_ray)
{
	t_vec3	light_dir;
	t_vec3	diffuse;
	t_vec3	specular;

	t_vec3	view_dir;
	t_vec3	reflect_dir;
	float	n_dot_l;
	float	spec_factor;

	if (!data->set_light || in_shadow(data, rec))
		return (make_vec(0.0f, 0.0f, 0.0f));
		
	light_dir = norm(sub(data->light.center, rec->p));
	n_dot_l = fmaxf(0.0f, dot(rec->normal, light_dir));
	diffuse = scale(rec->mat.tex.albedo, n_dot_l * data->light.brightness);

	view_dir = norm(scale(cam_ray.dir, -1.0f));

	// Formula: R = 2 * (N dot L) * N - L
	reflect_dir = sub(scale(rec->normal, 2.0f * n_dot_l), light_dir);
	reflect_dir = norm(reflect_dir);

	spec_factor = powf(fmaxf(0.0f, dot(view_dir, reflect_dir)), 10);

	specular = scale(make_vec(1.0f, 1.0f, 1.0f), spec_factor * data->light.brightness);

	diffuse = mult(diffuse, data->light.colour);
	specular = mult(specular, data->light.colour);
	return (add(diffuse, specular));
}

static t_vec3	get_ray_color_direct(t_data *data, t_ray ray)
{
	t_hit_record	rec;
	t_vec3			color;

	g_ray_count++;
	rec.set_face_normal = ft_set_face_normal;
	if (!data->world.hit(&data->world, ray,
			interval_init(0.001f, INFINITY), &rec))
		return (data->world.background);
	if (is_emissive(&rec))
		return (get_ray_color_direct(data,
				make_ray(rec.p, ray.dir)));
	color = mult(data->world.ambient, rec.mat.tex.albedo);
	color = add(color, compute_direct_light(data, &rec, ray));
	return (color);
}

int	render_frame_direct(t_data *data)
{
	t_ray		r;
	t_vec3		color;
	uint32_t	x;
	uint32_t	y;

	y = 0;
	while (y < data->height)
	{
		x = 0;
		while (x < data->width)
		{
			r = get_ray(data, x, y);
			color = get_ray_color_direct(data, r);
			write_color(data, x, y, color);
			x++;
		}
		y++;
	}
	return (0);
}
