/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:11:56 by timurray          #+#    #+#             */
/*   Updated: 2025/09/15 16:50:23 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strdup(const char *s)
{
	char	*strdup;
	size_t	len;

	len = ft_strlen(s);
	strdup = (char *)malloc((len + 1) * sizeof(char));
	if (strdup == NULL)
		return ((char *) NULL);
	ft_memcpy(strdup, s, len);
	strdup[len] = '\0';
	return (strdup);
}
