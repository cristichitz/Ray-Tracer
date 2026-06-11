/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_cpu.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:44:35 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 14:59:28 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

//TODO: too many vars
t_vec3	get_ray_color(t_hittable_list *world, int depth, t_ray ray)
{
	t_hit_record	hit_record;
	t_ray			scattered;
	t_vec3			attenuation;
	t_vec3			color_from_emission;
	t_vec3			color_from_ambient;
	t_vec3			color_from_scatter;

	hit_record.set_face_normal = ft_set_face_normal;
	g_ray_count++;
	if (depth <= 0)
		return (make_vec(0.0f, 0.0f, 0.0f));
	if (!world->hit(world, ray, interval_init(0.001f, INFINITY), &hit_record))
		return (world->background);
	color_from_emission = hit_record.mat.emitted(&hit_record.mat, hit_record.u,
			hit_record.v, hit_record.p);
	color_from_ambient = mult(world->ambient, hit_record.mat.tex.albedo);
	color_from_emission = add(color_from_emission, color_from_ambient);
	if (!hit_record.mat.scatter(&hit_record.mat, ray, hit_record, &attenuation,
			&scattered))
		return (color_from_emission);
	color_from_scatter = mult(attenuation, get_ray_color(world, depth - 1,
				scattered));
	return (add(color_from_emission, color_from_scatter));
}

t_vec3	sample_square(void)
{
	return (make_vec(random_float(0.0f, 1.0f) - 0.5, random_float(0.0f, 1.0f)
			- 0.5f, 0));
}

t_ray	get_ray(t_data *data, uint32_t x, uint32_t y)
{
	t_vec3	offset;
	t_vec3	pixel_sample;
	t_vec3	ray_origin;
	t_vec3	ray_direction;

	offset = sample_square();
	pixel_sample = scale(data->px_w, x + offset.x);
	pixel_sample = add(pixel_sample, scale(data->px_h, y + offset.y));
	pixel_sample = add(pixel_sample, data->pixel00_loc);
	ray_origin = data->origin;
	ray_direction = sub(pixel_sample, ray_origin);
	return (make_ray(ray_origin, ray_direction));
}

void	write_color(t_data *data, uint32_t x, uint32_t y, t_vec3 color)
{
	uint32_t	r;
	uint32_t	g;
	uint32_t	b;
	uint32_t	pixel_color;
	t_interval	intensity;

	intensity = interval_init(0.0f, 0.999f);
	r = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.x)));
	g = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.y)));
	b = (uint32_t)(256 * intensity.clamp(&intensity, sqrtf(color.z)));
	pixel_color = (r << 24) | (g << 16) | (b << 8) | 255;
	if (data->headless)
		data->fb[(size_t)y * data->width + x] = pixel_color;
	else
		mlx_put_pixel(data->img, x, y, pixel_color);
}

int	render_frame(t_data *data)
{
	t_ray	r;
	t_vec3	color;

	for (uint32_t y = 0; y < data->height; y++)
	{
		for (uint32_t x = 0; x < data->width; x++)
		{
			color = make_vec(0.0f, 0.0f, 0.0f);
			for (uint32_t sample = 0; sample < data->samples_per_pixel; sample++)
			{
				r = get_ray(data, x, y);
				color = add(color, get_ray_color(&data->world, data->max_depth,
							r));
			}
			write_color(data, x, y, scale(color, data->pixel_samples_scale));
		}
	}
	return (0);
}
