/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:34:42 by timurray          #+#    #+#             */
/*   Updated: 2026/03/04 15:35:33 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"

typedef struct s_ambient_light
{
	float			brightness;
	t_rgb			colour;
}					t_ambient_light;

typedef struct s_light
{
	t_pt			pt;
	float			brightness;
	t_rgb colour; // UNUSED in Mandatory but still parsed.
}					t_light;


#endif