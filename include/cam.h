/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cam.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 15:32:53 by timurray          #+#    #+#             */
/*   Updated: 2026/03/06 11:25:21 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAM_H
# define CAM_H

# include "types.h"

typedef struct s_cam
{
	t_vec3	center;
	t_vec3	uvec;
	float	yaw;
	float	pitch;
	int		fov;
	bool	assigned;
}			t_cam;

#endif