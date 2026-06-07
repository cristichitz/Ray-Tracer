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
# include <stdio.h>

typedef struct s_interval	t_interval;

typedef struct s_interval
{
	float					min;
	float					max;
	float					size;
	bool					(*contains)(t_interval *self, float x);
	bool					(*surrounds)(t_interval *self, float x);
	float					(*clamp)(t_interval *self, float x);
	t_interval				(*expand)(t_interval *self, float x);
}							t_interval;

t_interval					interval_init(float min, float max);

#endif
