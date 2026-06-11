/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_params_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:31:44 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:32:07 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	set_brightness(float *fnum, char *param)
{
	if (!get_float(fnum, param, 0.0, 1.0))
		return (0);
	return (1);
}
