/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:46:16 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:48:30 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static unsigned int	ft_num_len(long n);

char	*ft_itoa(int n)
{
	unsigned int	len;
	char			*numstr;
	long			nbr;

	nbr = (long)n;
	len = ft_num_len(nbr);
	numstr = (char *)malloc((len + 1) * sizeof(char));
	if (!numstr)
		return (NULL);
	numstr[len] = '\0';
	if (nbr == 0)
		numstr[0] = '0';
	if (nbr < 0)
	{
		numstr[0] = '-';
		nbr = -nbr;
	}
	while (nbr > 0)
	{
		len--;
		numstr[len] = (nbr % 10) + '0';
		nbr /= 10;
	}
	return (numstr);
}

static unsigned int	ft_num_len(long n)
{
	unsigned int	len;

	len = 0;
	if (n <= 0)
		len++;
	while (n != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}
