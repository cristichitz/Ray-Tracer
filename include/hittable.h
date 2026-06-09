/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:06:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/09 14:44:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "interval.h"
# include "libft.h"
# include "vec3.h"
# include <stdlib.h>
# include <sys/time.h>

typedef struct s_texture
{
	t_vec3					(*value)(struct s_texture *self, float u, float v,
							t_vec3 p);
	t_vec3					albedo;
}							t_texture;

t_texture					init_solid_color(t_vec3 col);

typedef struct s_ray		t_ray;
typedef struct s_hit_record	t_hit_record;
typedef struct s_material	t_material;

typedef struct s_material
{
	t_texture				tex;
	bool					(*scatter)(t_material *self, t_ray r_in,
							t_hit_record rec, t_vec3 *attenuation,
							t_ray *scattered);
	t_vec3					(*emmited)(t_material *self, float u, float v,
							t_vec3 p);
}							t_material;

t_material					init_lambertian(t_vec3 col);
t_material					init_metal(t_vec3 col);
t_material					init_diffuse_light(t_vec3 col);

typedef struct s_ray
{
	t_vec3					origin;
	t_vec3					dir;
	t_vec3					(*at)(struct s_ray *self, float t);
}							t_ray;

typedef struct s_hit_record
{
	t_vec3					p;
	t_vec3					normal;
	t_material				mat;
	float					t;
	float					u;
	float					v;
	bool					front_face;
	t_vec3					colour;
	void					(*set_face_normal)(struct s_hit_record *self,
							t_ray ray, t_vec3 outward_normal);
}							t_hit_record;

typedef struct s_hittable
{
	bool					(*hit)(void *object, t_ray ray, t_interval t,
							t_hit_record *rec);
	void					(*destroy)(void *object);
	void					(*resize)(void *object, float scalar);
	void					(*rotate)(void *object, t_vec3 axis, float angle);
}							t_hittable;

t_vec3						make_vec(float a, float b, float c);
t_ray						make_ray(t_vec3 origin, t_vec3 direction);

void						ft_set_face_normal(t_hit_record *self, t_ray r,
								t_vec3 outward_normal);
#endif
