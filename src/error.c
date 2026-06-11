/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/14 17:10:43 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:47:11 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_cpu.h"

void print_error(char *msg)
{
	ft_printfd(2,"Error\n%s\n",msg);
}

int return_print_error(char *msg, int err)
{
	print_error(msg);
	return (err);
}
