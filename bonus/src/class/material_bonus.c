/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:51 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:51 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

/*
** Materials are tagged by `type` so the kernel can branch instead of using
** function pointers: 0 = lambertian, 1 = metal, 2 = diffuse_light (emissive).
** For a diffuse_light, `albedo` doubles as the emitted colour.
*/
t_material	material_init(cl_float3 color, int type)
{
	t_material	mat;

	mat.albedo = color;
	mat.type = type;
	return (mat);
}
