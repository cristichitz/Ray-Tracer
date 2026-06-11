/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quaternion.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 17:11:19 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 15:02:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUATERNION_H
# define QUATERNION_H

# include "vec3.h"

typedef struct s_quaternion
{
	float		w;
	float		x;
	float		y;
	float		z;
}				t_quaternion;

t_quaternion	make_quaternion(t_vec3 axis, float angle);
t_quaternion	mult_quaternion(t_quaternion a, t_quaternion b);
t_quaternion	inverse_quaternion(t_quaternion q);
t_vec3			rotate_vec_by_quaternion(t_quaternion q, t_vec3 v);

#endif
