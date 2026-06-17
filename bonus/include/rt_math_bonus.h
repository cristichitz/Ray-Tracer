/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_math_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 16:30:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 16:30:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_MATH_BONUS_H
# define RT_MATH_BONUS_H

# include "rt_types_bonus.h"

float					random_float(float min, float max);
float					degrees_to_radians(float degrees);

cl_float3				make_float3(cl_float x, cl_float y, cl_float z);

float					length_squared(cl_float3 a);
cl_float3				random_vec_between(float min, float max);
cl_float3				random_unit_vector(void);
cl_float3				random_on_hemisphere(cl_float3 normal);

bool					near_zero(cl_float3 a);
cl_float3				mult(cl_float3 a, cl_float3 b);

cl_float3				make_vec(float a, float b, float c);
cl_float3				add(cl_float3 a, cl_float3 b);
cl_float3				sub(cl_float3 a, cl_float3 b);
cl_float3				scale(cl_float3 a, float b);
cl_float3				divide(cl_float3 a, float b);
float					dot(cl_float3 a, cl_float3 b);
cl_float3				cross(cl_float3 a, cl_float3 b);
cl_float3				norm(cl_float3 a);
void					print_vec(cl_float3 a);

#endif
