/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:04:14 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:26:27 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

void	replace_tabs(char *str)
{
	while (*str)
	{
		if (*str == '\t')
			*str = ' ';
		str++;
	}
}

size_t	split_len(char **split)
{
	size_t	len;

	len = 0;
	if (!split)
		return (0);
	while (split[len])
		len++;
	return (len);
}

int	split_count(char **params, size_t expected)
{
	if (split_len(params) != expected)
	{
		print_error("Invalid number of arguments.");
		return (0);
	}
	return (1);
}

int	valid_csv(char *str, int expected_commas)
{
	int	count;

	count = 0;
	while (*str)
	{
		if (*str == ',')
			count++;
		str++;
	}
	if (count != expected_commas)
	{
		print_error("Invalid comma-separated format.");
		return (0);
	}
	return (1);
}

int	force_normalised(t_vec3 *v)
{
	float	len_sq;

	len_sq = length_squared(*v);
	if (len_sq < 1e-12f)
	{
		print_error("Orientation vector cannot be zero.");
		return (0);
	}
	*v = norm(*v);
	return (1);
}
