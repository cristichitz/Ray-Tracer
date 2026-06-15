/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_preset_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:00:00 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 18:23:37 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"

/*
** Named "physics materials": one keyword in the .rt file picks both the render
** look (type) and the rigid-body behaviour (dynamic flag, density -> mass,
** restitution, friction). The visible colour is passed separately, so e.g.
** "brick" + any rgb is a heavy, barely-bouncy box of that colour. phys packs
** (density, restitution, friction) to stay inside the argument limit.
*/
static t_material	mat_make(cl_float3 color, int type, int dyn, cl_float3 phys)
{
	t_material	mat;

	mat.albedo = color;
	mat.type = type;
	mat.dynamic = dyn;
	mat.density = phys.x;
	mat.restitution = phys.y;
	mat.friction = phys.z;
	mat.portal = -1;
	return (mat);
}

/* Dynamic presets: anything that should fall, tumble and collide. */
static int	dynamic_preset(const char *name, cl_float3 c, t_material *out)
{
	if (!ft_strcmp(name, "metal"))
		*out = mat_make(c, 1, 1, make_float3(3.0f, 0.15f, 0.40f));
	else if (!ft_strcmp(name, "glass"))
		*out = mat_make(c, 1, 1, make_float3(1.2f, 0.30f, 0.20f));
	else if (!ft_strcmp(name, "brick"))
		*out = mat_make(c, 0, 1, make_float3(1.0f, 0.10f, 0.60f));
	else if (!ft_strcmp(name, "wood"))
		*out = mat_make(c, 0, 0, make_float3(0.6f, 0.20f, 0.50f));
	else if (!ft_strcmp(name, "rubber"))
		*out = mat_make(c, 0, 1, make_float3(0.5f, 0.80f, 0.90f));
	else
		return (0);
	return (1);
}

/*
** Resolve a material keyword (+ colour) to a full material. Unknown or NULL
** names and the static keywords (wall/floor/matte) give an immovable matte
** surface; "light" is a static emitter. Static surfaces still collide (as
** colliders) but never move.
*/
t_material	material_named(const char *name, cl_float3 color)
{
	t_material	mat;

	if (name && dynamic_preset(name, color, &mat))
		return (mat);
	if (name && !ft_strcmp(name, "light"))
		return (mat_make(scale(color, LIGHT_GAIN), 2, 0,
				make_float3(1.0f, 0.0f, 0.0f)));
	return (mat_make(color, 0, 0, make_float3(1.0f, 0.20f, 0.50f)));
}
