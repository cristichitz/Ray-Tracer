/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 14:24:58 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:49:20 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static int	ft_isset(char c, const char *set)
{
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	trim_start;
	size_t	trim_end;
	size_t	trim_size;
	char	*s;

	if (!s1 || !set)
		return (NULL);
	trim_start = 0;
	trim_end = ft_strlen(s1) - 1;
	while (ft_isset(*(s1 + trim_start), set))
		trim_start++;
	while ((ft_isset(*(s1 + trim_end), set)) && trim_end > 0)
		trim_end--;
	if (trim_end < trim_start)
		return (ft_strdup(""));
	trim_size = trim_end - trim_start + 1;
	s = (char *)malloc((trim_size) * sizeof(char) + 1);
	if (!s)
		return (NULL);
	ft_memcpy(s, (s1 + trim_start), trim_size);
	*(s + trim_size) = '\0';
	return (s);
}
