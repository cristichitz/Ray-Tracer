/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 12:29:27 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:49:06 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	src_length;
	size_t	dst_length;
	int		src_index;
	size_t	dst_offset;

	src_length = ft_strlen(src);
	dst_length = ft_strlen(dst);
	if (dstsize <= dst_length)
		return (dstsize + src_length);
	dst_offset = dst_length;
	src_index = 0;
	while (*(src + src_index) && (dst_offset < dstsize - 1))
		*(dst + dst_offset++) = *(src + src_index++);
	*(dst + dst_offset) = '\0';
	return (src_length + dst_length);
}
