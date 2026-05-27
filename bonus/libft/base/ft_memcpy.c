/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaeklee <jaeklee@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 11:40:13 by timurray          #+#    #+#             */
/*   Updated: 2025/11/06 18:08:00 by jaeklee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*temp_dest;
	const unsigned char	*temp_src;

	temp_dest = dest;
	temp_src = (const unsigned char *)src;
	while (n > 0)
	{
		*temp_dest++ = *temp_src++;
		n--;
	}
	return (dest);
}
