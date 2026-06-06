/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resize.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 16:49:21 by timurray          #+#    #+#             */
/*   Updated: 2026/06/06 11:33:25 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MLX42/MLX42.h"
#include "rt_cpu.h"

bool resize_object(t_data *data, float *scalar) {
  t_hittable *object;
  bool resized;

  resized = false;
  if (data->world.objects->len > 0) {
    object = (t_hittable *)ft_vec_get(
        data->world.objects, data->object_i);
    if (object->resize) {
      if (mlx_is_key_down(data->mlx, MLX_KEY_KP_ADD))
        resized = (object->resize(object, *scalar), true);
      if (mlx_is_key_down(data->mlx, MLX_KEY_KP_SUBTRACT))
        resized = (object->resize(object, -(*scalar)), true);
    }
  }
  return (resized);
}
