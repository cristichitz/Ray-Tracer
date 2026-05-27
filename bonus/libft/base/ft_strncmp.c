/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 12:25:26 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:49:13 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;
	size_t	result;

	i = 0;
	result = 0;
	while ((*s1 || *s2) && (i < n))
	{
		result = *s1++ - *s2++;
		if (result != 0)
			return (result);
		i++;
	}
	return (result);
}
