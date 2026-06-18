/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quality.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 16:34:21 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 15:46:14 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

void	set_quality(t_data *data, t_quality quality)
{
	if (quality == HIGH)
	{
		data->samples_per_pixel = 50;
		data->max_depth = 15;
		data->pixel_samples_scale = 1.0f / data->samples_per_pixel;
		data->render_check = true;
		return ;
	}
	if (quality == LOW)
	{
		data->samples_per_pixel = 1;
		data->pixel_samples_scale = 1.0f;
		data->max_depth = 2;
		return ;
	}
}
