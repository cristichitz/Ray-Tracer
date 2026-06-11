/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quarternion.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:11:19 by timurray          #+#    #+#             */
/*   Updated: 2026/06/08 18:57:45 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUARTERNION_H
# define QUARTERNION_H

# include "vec3.h"

typedef struct s_quarternion
{
	float		w;
	float		x;
	float		y;
	float		z;
}				t_quarternion;

t_quarternion	make_quarternion(t_vec3 axis, float angle);
t_quarternion	mult_quarternion(t_quarternion a, t_quarternion b);
t_quarternion	inverse_quartenion(t_quarternion q);
t_vec3			rotate_vec_by_quarternion(t_quarternion q, t_vec3 v);

#endif