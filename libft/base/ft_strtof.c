/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtof.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:09:41 by timurray          #+#    #+#             */
/*   Updated: 2026/02/08 16:33:30 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

float	ft_strtof(const char *nptr, char **endptr)
{
	unsigned int	i;
	int				sign;
	int				e_sign;
	double			num;
	double			scale;
	int				e;

	i = 0;
	while (ft_isspace(nptr[i]))
		i++;
	sign = 1;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	num = 0.0;
	while (ft_isdigit(nptr[i]))
	{
		num = num * 10.0 + (nptr[i] - '0');
		i++;
	}
	if (nptr[i] == '.')
	{
		i++;
		while (ft_isdigit(nptr[i]))
		{
			num = num * 10.0 + (nptr[i] - '0');
			scale *= 10.0;
			i++;
		}
	}
	num = num / scale;
	if (nptr[i] == 'e' || nptr == 'E')
	{
		i++;
		if (nptr[i] == '+' || nptr[i] == '-')
		{
			if (nptr[i] == '-')
				e_sign = -1;
			i++;
		}
		while (ft_isdigit(nptr[i]))
		{
			e = e * 10 + (nptr[i] - '0');
			i++;
		}
	}
	while (e-- > 0)
	{
		if (e_sign == 1)
			num *= 10.0;
		else
			num /= 10.0;
	}
	if (endptr)
		*endptr = nptr[i];
	return (float)(sign * num);
}
