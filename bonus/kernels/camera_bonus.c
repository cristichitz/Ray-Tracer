/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:47:29 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:47:38 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

float3	sample_square(uint *seed)
{
	return ((float3)(random_float(0.0f, 1.0f, seed) - 0.5f, random_float(0.0f,
			1.0f, seed) - 0.5f, 0.0f));
}

t_ray	get_ray(t_image *frame, int x, int y, uint *seed)
{
	float3	offset;
	float3	pixel_sample;
	float3	ray_origin;
	float3	ray_dir;

	offset = sample_square(seed);
	pixel_sample = (frame->pixel_delta_u * (x + offset.x))
		+ (frame->pixel_delta_v * (y + offset.y) + frame->pixel00_loc);
	ray_origin = frame->origin;
	ray_dir = pixel_sample - ray_origin;
	return (make_ray(ray_origin, ray_dir));
}
