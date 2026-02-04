/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_num_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:47:01 by timurray          #+#    #+#             */
/*   Updated: 2025/10/04 13:48:45 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static ssize_t	ft_str_len(const char *s)
{
	ssize_t	length;

	length = 0;
	while (*s++)
		length += 1;
	return (length);
}

ssize_t	num_handler(const long num, const char *s)
{
	ssize_t	count;
	ssize_t	count2;
	long	base_length;

	base_length = (long)ft_str_len(s);
	if (num < 0)
	{
		if (ft_putchar_fd('-', 1) == -1)
			return (-1);
		return (num_handler(-num, s) + 1);
	}
	else if (num < base_length)
	{
		if (ft_putchar_fd(s[num], 1) == -1)
			return (-1);
		return (1);
	}
	else
	{
		count = num_handler(num / base_length, s);
		count2 = num_handler(num % base_length, s);
		if ((count2 == -1) || (count == -1))
			return (-1);
		return (count2 + count);
	}
}

ssize_t	unum_handler(const unsigned long long num, const char *s)
{
	ssize_t				count;
	ssize_t				count2;
	unsigned long long	base_length;

	base_length = (unsigned long long)ft_str_len(s);
	if (num < base_length)
	{
		if (ft_putchar_fd(s[num], 1) == -1)
			return (-1);
		return (1);
	}
	else
	{
		count = unum_handler(num / base_length, s);
		if (count == -1)
			return (-1);
		count2 = unum_handler(num % base_length, s);
		if (count2 == -1)
			return (-1);
		return (count2 + count);
	}
}

ssize_t	ptr_handler(const void *ptr, const char *s)
{
	ssize_t	count;
	ssize_t	count2;

	count = 0;
	if (!ptr)
		return (ft_putstr_fd("(nil)", 1));
	else
	{
		count = str_handler("0x");
		if (count == -1)
			return (-1);
		count2 = unum_handler((uintptr_t)ptr, s);
		if (count2 == -1)
			return (-1);
		return (count + count2);
	}
}
