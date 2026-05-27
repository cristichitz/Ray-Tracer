/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 12:26:54 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:48:40 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*tdest;
	unsigned char	*tsrc;

	tdest = (unsigned char *)dest;
	tsrc = (unsigned char *)src;
	if (tdest <= tsrc)
	{
		while (n--)
			*tdest++ = *tsrc++;
	}
	else if (tdest > tsrc)
	{
		tdest = tdest + (n - 1);
		tsrc = tsrc + (n - 1);
		while (n--)
			*tdest-- = *tsrc--;
	}
	return (dest);
}
