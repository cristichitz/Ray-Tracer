/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:35:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 17:35:00 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATERIAL_H
# define MATERIAL_H

# include "ray.h"
# include "vec3.h"

typedef struct s_texture
{
	t_vec3					(*value)(struct s_texture *self, float u, float v,
							t_vec3 p);
	t_vec3					albedo;
}							t_texture;

t_texture					init_solid_color(t_vec3 col);

typedef struct s_hit_record	t_hit_record;
typedef struct s_material	t_material;

typedef struct s_material
{
	t_texture				tex;
	bool					(*scatter)(t_material *self, t_ray r_in,
							t_hit_record rec, t_vec3 *attenuation,
							t_ray *scattered);
	t_vec3					(*emitted)(t_material *self, float u, float v,
							t_vec3 p);
}							t_material;

t_material					init_lambertian(t_vec3 col);
t_material					init_metal(t_vec3 col);
t_material					init_diffuse_light(t_vec3 col);

#endif
