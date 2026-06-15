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
** For a diffuse_light, `albedo` doubles as the emitted colour. A bare material
** is static (dynamic = 0) so legacy scenes keep rendering as immovable
** geometry; material_named() (material_preset_bonus.c) is what turns the
** physics on.
*/
t_material	material_init(cl_float3 color, int type)
{
	t_material	mat;

	mat.albedo = color;
	mat.type = type;
	mat.dynamic = 0;
	mat.density = 1.0f;
	mat.restitution = PHYS_RESTITUTION;
	mat.friction = PHYS_FRICTION;
	mat.portal = -1;
	return (mat);
}
