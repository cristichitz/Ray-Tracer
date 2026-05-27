/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 14:04:44 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:49:03 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_length;
	size_t	s2_length;
	size_t	s_size;
	char	*s;

	if (!s1 || !s2)
		return (NULL);
	s1_length = ft_strlen(s1);
	s2_length = ft_strlen(s2);
	s_size = s1_length + s2_length + 1;
	s = (char *)malloc((s_size) * sizeof(char));
	if (!s)
		return (NULL);
	ft_memcpy(s, s1, s1_length);
	ft_memcpy(s + s1_length, s2, s2_length);
	s[s_size - 1] = '\0';
	return (s);
}
