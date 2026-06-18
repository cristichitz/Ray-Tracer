/* ************************************************************************** */
/*                                                                            */
/*   metal_bridge.h                                                           */
/*                                                                            */
/*   C-callable API exposed by the Objective-C Metal bridge.                  */
/*   Include this from C files; the implementation lives in metal_bridge.m.  */
/*                                                                            */
/* ************************************************************************** */

#ifndef METAL_BRIDGE_H
# define METAL_BRIDGE_H

/*
** metal_init() — call once after the MLX42 window is created.
** Loads render.metallib, builds the compute pipeline, allocates
** shared Metal buffers (camera data, object descriptors, pixel output).
** Returns 1 on success, 0 on failure (logs reason to stderr).
** On failure, subsequent render_frame_metal() calls are no-ops.
*/
int		metal_init(void);

/*
** render_frame_metal(data) — render one frame on the GPU.
** Packs t_data into GPU-friendly structs, dispatches the Metal compute
** kernel (one thread per pixel), waits for completion, then copies the
** finished RGBA pixels into data->img->pixels for MLX42 to display.
**
** param: pointer to t_data (passed as void* to avoid Metal headers leaking
**        into pure-C translation units that include this header).
*/
void	render_frame_metal(void *data);

/*
** metal_cleanup() — release all Metal resources.
** Call once before mlx_terminate() returns.
*/
void	metal_cleanup(void);

#endif
