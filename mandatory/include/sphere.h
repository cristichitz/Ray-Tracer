/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:07:59 by timurray          #+#    #+#             */
/*   Updated: 2026/06/10 16:46:47 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPHERE_H
# define SPHERE_H

# include "hittable.h"

typedef struct s_sphere
{
	t_hittable	base;
	t_vec3		center;
	float		radius;
	t_material	mat;
}				t_sphere;

typedef struct s_sphere_var
{
	float		a;
	float		h;
	float		c;
	float		discriminant;
	float		sqrtd;
	float		root;
} t_sphere_var;


t_sphere		*make_sphere(t_sphere sphere);

#endif
