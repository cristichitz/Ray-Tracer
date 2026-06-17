/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:35:00 by timurray          #+#    #+#             */
/*   Updated: 2026/06/13 21:01:05 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATERIAL_H
# define MATERIAL_H

# include "ray.h"
# include "vec3.h"

typedef struct s_texture	t_texture;

typedef t_vec3				(*t_tex_value)(t_texture *self, float u, float v,
					t_vec3 p);

typedef struct s_texture
{
	t_tex_value				value;
	t_vec3					albedo;
}							t_texture;

typedef struct s_scat
{
	t_vec3					attenuation;
	t_ray					scattered;
}							t_scat;

t_texture					init_solid_color(t_vec3 col);

typedef struct s_hit_record	t_hit_record;
typedef struct s_material	t_material;

typedef bool				(*t_scatter)(t_material *self, t_ray r_in,
					t_hit_record rec, t_scat *scat);
typedef t_vec3				(*t_emitted)(t_material *self, float u, float v,
					t_vec3 p);

typedef enum e_mat_type
{
	MAT_LAMBERTIAN,
	MAT_METAL,
	MAT_LIGHT
}								t_mat_type;

typedef struct s_material
{
	t_texture				tex;
	t_scatter				scatter;
	t_emitted				emitted;
	t_mat_type				type;
}							t_material;

t_material					init_lambertian(t_vec3 col);
t_material					init_metal(t_vec3 col);
t_material					init_diffuse_light(t_vec3 col);

#endif
