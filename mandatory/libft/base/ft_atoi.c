/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaeklee <jaeklee@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 12:59:45 by timurray          #+#    #+#             */
/*   Updated: 2025/09/24 17:03:01 by jaeklee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_atoi(const char *nptr)
{
	int	sign;
	int	n;
	int	index;

	index = 0;
	sign = 1;
	n = 0;
	while (ft_isspace(*(nptr + index)))
		index++;
	if (nptr[index] == '-' || nptr[index] == '+')
	{
		if (nptr[index] == '-')
			sign = -1;
		index++;
	}
	while (ft_isdigit(*(nptr + index)))
	{
		n = n * 10 + (*(nptr + index) - '0');
		index++;
	}
	return (n * sign);
}
