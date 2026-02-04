/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_check.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaeklee <jaeklee@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 08:09:27 by timurray          #+#    #+#             */
/*   Updated: 2025/09/24 17:04:37 by jaeklee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static int	limit_check(long long n, int sign, int digit);

int	ft_atoi_check(const char *nptr, int *num)
{
	int			sign;
	long long	n;
	int			i;
	int			digit;

	sign = 1;
	n = 0;
	i = 0;
	while (ft_isspace(nptr[i]))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i++] == '-')
			sign = -1;
	}
	while (ft_isdigit(nptr[i]))
	{
		digit = nptr[i++] - '0';
		if (!limit_check(n, sign, digit))
			return (0);
		n = (n * 10) + digit;
	}
	*num = (int)(sign * n);
	return (1);
}

static int	limit_check(long long n, int sign, int digit)
{
	long long	limit;

	if (sign > 0)
		limit = (long long)INT_MAX;
	else
		limit = -(long long)INT_MIN;
	if (n > (limit - digit) / 10)
		return (0);
	return (1);
}
