/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:06:51 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 18:06:55 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "interval.h"
# include "libft.h"
# include "vec3.h"
# include <stdlib.h>
# include <sys/time.h>

typedef struct s_ray		t_ray;
typedef struct s_hit_record	t_hit_record;

typedef struct s_material
{
	t_vec3					albedo;
	bool					(*scatter)(struct s_material *self, t_ray r_in,
							t_hit_record rec, t_vec3 *attenuation,
							t_ray *scattered);
}							t_material;

t_material					init_lambertian(t_vec3 col);
t_material					init_metal(t_vec3 col);

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
	float					t;
	bool					front_face;
	t_vec3					colour;
	t_material				mat;
	void					(*set_face_normal)(struct s_hit_record *self,
							t_ray ray, t_vec3 outward_normal);
}							t_hit_record;

// Base class
typedef struct s_hittable
{
	bool					(*hit)(void *object, t_ray ray, t_interval t,
							t_hit_record *rec);
	void					(*destroy)(void *object);
	void					(*resize)(void *object, float scalar);
}							t_hittable;

t_vec3						make_vec(float a, float b, float c);
t_ray						make_ray(t_vec3 origin, t_vec3 direction);

void						ft_set_face_normal(t_hit_record *self, t_ray r,
								t_vec3 outward_normal);
#endif
