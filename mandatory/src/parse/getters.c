/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:01:03 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:29:15 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	get_float(float *num, char *param, float min, float max)
{
	float	fnum;
	char	*endptr;

	endptr = NULL;
	fnum = ft_strtof(param, &endptr);
	if (!endptr || *endptr != '\0' || endptr == param)
		return (return_print_error("Invalid value.", 0));
	if (fnum > max)
		return (return_print_error("Value too large.", 0));
	if (fnum < min)
		return (return_print_error("Value too low.", 0));
	*num = fnum;
	return (1);
}

int	get_int(int *num, char *param, int min, int max)
{
	int	n;
	int	i;

	i = 0;
	if (param[i] == '-' || param[i] == '+')
		i++;
	if (!param[i])
		return (return_print_error("Invalid integer.", 0));
	while (param[i])
	{
		if (!ft_isdigit(param[i]))
			return (return_print_error("Invalid integer.", 0));
		i++;
	}
	if (!ft_atoi_check(param, &n))
		return (return_print_error("Integer overflow.", 0));
	if (n > max)
		return (return_print_error("Value too large.", 0));
	if (n < min)
		return (return_print_error("Value too low.", 0));
	*num = n;
	return (1);
}

int	get_pt(float *num, char *param)
{
	if (!get_float(num, param, COORD_MIN, COORD_MAX))
		return (0);
	return (1);
}

int	get_uvec_pt(float *num, char *param)
{
	if (!get_float(num, param, -1.0, 1.0))
		return (0);
	return (1);
}

int	get_normed_float(float *num, char *param, float min, float max)
{
	int	inum;

	if (!get_int(&inum, param, (int)min, (int)max))
		return (0);
	*num = (float)inum / 255.0f;
	return (1);
}
