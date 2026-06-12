/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:46 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:47 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Host-side builders for the flat t_object array uploaded to the GPU.
** The actual ray/primitive intersection math lives in the kernels
** (bonus/kernels/hit_sphere_bonus.c and friends); here we only fill structs.
*/
t_object	make_obj_sphere(cl_float3 center, float radius, t_material mat)
{
	t_object	s;

	s.type = OBJ_SPHERE;
	s.center = center;
	s.radius = radius;
	s.material = mat;
	return (s);
}
