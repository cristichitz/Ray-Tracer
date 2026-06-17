/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_params.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:07:29 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:29:44 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	set_colour(t_vec3 *colour, char *params)
{
	char	**parts;
	int		ok;

	if (!valid_csv(params, 2))
		return (0);
	parts = ft_split(params, ',');
	if (!parts)
		return (0);
	ok = 1;
	if (split_len(parts) != 3)
		ok = 0;
	else if (!get_normed_float(&colour->x, parts[0], 0, 255.0f)
		|| !get_normed_float(&colour->y, parts[1], 0, 255.0f)
		|| !get_normed_float(&colour->z, parts[2], 0, 255.0f))
		ok = 0;
	ft_free_split(parts);
	return (ok);
}

int	set_material(t_material *mat, char *params)
{
	t_vec3	colour;

	colour = make_vec(0.0f, 0.0f, 0.0f);
	if (!set_colour(&colour, params))
		return (0);
	*mat = init_lambertian(colour);
	return (1);
}

int	set_radius(float *num, char *param)
{
	float	diameter;

	if (!get_float(&diameter, param, 0.001, FLT_MAX))
		return (0);
	*num = diameter / 2.0f;
	return (1);
}

int	set_height(float *num, char *param)
{
	if (!get_float(num, param, 0.001, FLT_MAX))
		return (0);
	return (1);
}

int	set_fov(int *num, char *param)
{
	if (!get_int(num, param, 0, 180))
		return (0);
	return (1);
}
