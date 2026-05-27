/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_char_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:47:01 by timurray          #+#    #+#             */
/*   Updated: 2025/12/13 11:17:57 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

ssize_t	ft_putchar_fd(char c, int fd)
{
	return (write(fd, &c, 1));
}

ssize_t	ft_putstr_fd(const char *s, int fd)
{
	size_t	len;

	if (!s)
		return (-1);
	len = ft_strlen(s);
	return (write(fd, s, len));
}

int	char_handler(const int c)
{
	return (ft_putchar_fd(c, 1));
}

ssize_t	str_handler(const char *s)
{
	if (!s)
		return (ft_putstr_fd("(null)", 1));
	else
		return (ft_putstr_fd(s, 1));
}
