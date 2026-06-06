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

void resize_object(t_data *data, float *scalar) {
  t_hittable *object;

  if (data->world.objects->len > 0) {
    object = (t_hittable *)ft_vec_get(
        data->world.objects, data->object_i % data->world.objects->len);
    if (object->resize) {
      if (mlx_is_key_down(data->mlx, MLX_KEY_KP_ADD))
        object->resize(object, *scalar);
      if (mlx_is_key_down(data->mlx, MLX_KEY_KP_SUBTRACT))
        object->resize(object, -(*scalar));
    }
  }
}
