/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 15:01:44 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:48:42 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*mem;
	unsigned char	fill;

	mem = (unsigned char *)s;
	fill = (unsigned char)c;
	while (n > 0)
	{
		*mem++ = fill;
		n--;
	}
	return (s);
}
