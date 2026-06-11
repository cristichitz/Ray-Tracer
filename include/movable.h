/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   movable.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:23:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 13:40:22 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOVABLE_H
# define MOVABLE_H

# include "hittable.h"

typedef struct s_movable
{
	t_hittable	base;
	t_vec3		center;
}				t_movable;

#endif