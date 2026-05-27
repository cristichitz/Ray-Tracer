/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 15:00:13 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:48:35 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*s1temp;
	const unsigned char	*s2temp;

	s1temp = (const unsigned char *)s1;
	s2temp = (const unsigned char *)s2;
	while (n--)
	{
		if (*s1temp != *s2temp)
			return (*s1temp - *s2temp);
		s1temp++;
		s2temp++;
	}
	return (0);
}
