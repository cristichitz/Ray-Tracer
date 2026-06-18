/* ************************************************************************** */
/*                                                                            */
/*   metal_bonus_bridge.h                                                     */
/*                                                                            */
/*   C-visible header for the Objective-C Metal GPU bridge (bonus).           */
/*   Provides the same init/render/cleanup API the bonus expects, but         */
/*   backed by Apple Metal instead of OpenCL.                                 */
/*                                                                            */
/* ************************************************************************** */

#ifndef METAL_BONUS_BRIDGE_H
# define METAL_BONUS_BRIDGE_H

/* Forward-declare t_data so we do not pull in rt_types_bonus.h here. */
typedef struct s_data	t_data;

int		metal_bonus_init(t_data *data);
void	metal_bonus_render(t_data *data);
void	metal_bonus_cleanup(t_data *data);
void	metal_bonus_resize(t_data *data);
void	metal_bonus_upload_scene(t_data *data);

#endif
