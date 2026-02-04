/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:35:40 by timurray          #+#    #+#             */
/*   Updated: 2025/09/23 15:18:44 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*vec_find(t_vec *src, bool (*f)(void *))
{
	void	*ptr;
	size_t	i;

	if (!src || !src->memory)
		return (NULL);
	i = 0;
	while (i < src->len)
	{
		ptr = ft_vec_get(src, i);
		if (f(ptr) == true)
			return (ptr);
		i++;
	}
	return (NULL);
}
