/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtof.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:09:41 by timurray          #+#    #+#             */
/*   Updated: 2026/02/09 12:41:58 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static inline int	handle_sign(const char **nptr)
{
	int	sign;

	sign = 1;
	if (**nptr == '+' || **nptr == '-')
	{
		if (**nptr == '-')
			sign = -1;
		(*nptr)++;
	}
	return (sign);
}

static inline void	handle_scale(const char **nptr, double *num)
{
	double	scale;

	scale = 1.0;
	if (**nptr == '.')
	{
		(*nptr)++;
		while (ft_isdigit(**nptr))
		{
			*num = *num * 10.0 + (**nptr - '0');
			scale *= 10.0;
			(*nptr)++;
		}
	}
	*num /= scale;
}

static inline void	handle_exp(const char **nptr, double *num)
{
	int	e_sign;
	int	e;

	e_sign = 1;
	e = 0;
	if (**nptr == 'e' || **nptr == 'E')
	{
		(*nptr)++;
		e_sign = handle_sign(nptr);
		while (ft_isdigit(**nptr))
		{
			e = e * 10 + (**nptr - '0');
			(*nptr)++;
		}
	}
	while (e-- > 0)
	{
		if (e_sign == 1)
			*num *= 10.0;
		else
			*num /= 10.0;
	}
}

float	ft_strtof(const char *nptr, char **endptr)
{
	int		sign;
	int		e_sign;
	double	num;
	int		e;

	while (ft_isspace(*nptr))
		nptr++;
	sign = handle_sign(&nptr);
	num = 0.0;
	while (ft_isdigit(*nptr))
	{
		num = num * 10.0 + (*nptr - '0');
		nptr++;
	}
	handle_scale(&nptr, &num);
	handle_exp(&nptr, &num);
	if (endptr)
		*endptr = (char *)nptr;
	return (float)(sign * num);
}
