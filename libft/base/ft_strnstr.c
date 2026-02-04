/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:44:09 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:49:16 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (ft_strlen(little) == 0)
		return ((char *)big);
	while ((*(big + i)) && (i < len))
	{
		j = 0;
		if (*(big + i) == *(little + j))
		{
			while ((i + j) < len && *(big + i + j) == *(little + j))
				j++;
			if (*(little + j) == '\0')
				return ((char *)big + i);
		}
		i++;
	}
	return (NULL);
}
