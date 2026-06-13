/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_cpu.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:44:35 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 20:08:47 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

// TODO: too many vars
t_vec3	get_ray_color(t_hittable_list *world, int depth, t_ray ray)
{
	t_hit_record	hit_record;
	t_scat			scat;
	t_vec3			col_emis_amb_scat[3];

	hit_record.set_face_normal = ft_set_face_normal;
	if (depth <= 0)
		return (make_vec(0.0f, 0.0f, 0.0f));
	if (!world->hit(world, ray, interval_init(0.001f, INFINITY), &hit_record))
		return (world->background);
	col_emis_amb_scat[0] = hit_record.mat.emitted(&hit_record.mat, hit_record.u,
			hit_record.v, hit_record.p);
	col_emis_amb_scat[1] = mult(world->ambient, hit_record.mat.tex.albedo);
	col_emis_amb_scat[0] = add(col_emis_amb_scat[0], col_emis_amb_scat[1]);
	if (!hit_record.mat.scatter(&hit_record.mat, ray, hit_record, &scat))
		return (col_emis_amb_scat[0]);
	col_emis_amb_scat[2] = mult(scat.attenuation, get_ray_color(world, depth - 1,
				scat.scattered));
	return (add(col_emis_amb_scat[0], col_emis_amb_scat[2]));
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

t_vec3	gather_samples(t_data *data, uint32_t x, uint32_t y)
{
	uint32_t	sample;
	t_ray		r;
	t_vec3		c;

	sample = 0;
	c = make_vec(0.0f, 0.0f, 0.0f);
	while (sample < data->samples_per_pixel)
	{
		r = get_ray(data, x, y);
		c = add(c, get_ray_color(&data->world, data->max_depth, r));
		sample++;
	}
	return (c);
}

int	render_frame(t_data *data)
{
	t_vec3		c;
	uint32_t	y;
	uint32_t	x;

	y = 0;
	while (y < data->height)
	{
		x = 0;
		while (x < data->width)
		{
			c = gather_samples(data, x, y);
			write_color(data, x, y, scale(c, data->pixel_samples_scale));
			x++;
		}
		y++;
	}
	return (0);
}
