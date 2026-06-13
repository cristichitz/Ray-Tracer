/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 18:07:01 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 18:07:16 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERVAL_H
# define INTERVAL_H

# include <stdbool.h>

typedef struct s_interval	t_interval;

typedef bool				(*t_itv_contains)(t_interval *self, float x);
typedef bool				(*t_itv_surrounds)(t_interval *self, float x);
typedef float				(*t_itv_clamp)(t_interval *self, float x);
typedef t_interval			(*t_itv_expand)(t_interval *self, float x);

typedef struct s_interval
{
	float					min;
	float					max;
	float					size;
	t_itv_contains			contains;
	t_itv_surrounds			surrounds;
	t_itv_clamp				clamp;
	t_itv_expand			expand;
}							t_interval;

t_interval					interval_init(float min, float max);

#endif
