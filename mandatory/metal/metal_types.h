/* ************************************************************************** */
/*                                                                            */
/*   metal_types.h                                                            */
/*                                                                            */
/*   Shared data layout for CPU↔GPU communication.                           */
/*   Must stay in sync with the struct definitions in render.metal.           */
/*                                                                            */
/*   Layout rules: all members are float (4B) or uint32_t (4B), so both C   */
/*   and MSL agree on alignment/padding — zero surprises on M2 UMA.          */
/*                                                                            */
/* ************************************************************************** */

#ifndef METAL_TYPES_H
# define METAL_TYPES_H

# include <stdint.h>

/*
** A 3-component vector using explicit scalar members so that both C
** (struct { float x,y,z; }) and MSL (same layout, 4-byte aligned)
** produce identical 12-byte, 4-byte-aligned blocks.
** Do NOT use float[3] — array vs struct member rules can differ.
*/
typedef struct s_mfloat3
{
	float	x;
	float	y;
	float	z;
}	t_mfloat3;

/*
** Camera + render-settings uploaded once per frame.
** Offset map (all 4-byte aligned, no padding needed):
**   0   origin        12B
**   12  pixel00       12B
**   24  px_w          12B
**   36  px_h          12B
**   48  ambient       12B
**   60  background    12B
**   72  light_pos     12B
**   84  light_color   12B
**   96  light_brightness 4B
**   100 has_light     4B
**   104 width         4B
**   108 height        4B
**   112 max_depth     4B
**   116 samples_per_pixel 4B
**   120 frame_count   4B
**   Total: 124B
*/
typedef struct s_metal_cam
{
	t_mfloat3	origin;
	t_mfloat3	pixel00;
	t_mfloat3	px_w;
	t_mfloat3	px_h;
	t_mfloat3	ambient;
	t_mfloat3	background;
	t_mfloat3	light_pos;
	t_mfloat3	light_color;
	float		light_brightness;
	uint32_t	has_light;
	uint32_t	width;
	uint32_t	height;
	uint32_t	max_depth;
	uint32_t	samples_per_pixel;
	uint32_t	frame_count;
}	t_metal_cam;

/* Object type tags written by metal_bridge.m and read by the GPU kernel. */
# define MOBJ_SPHERE    1u
# define MOBJ_CYLINDER  2u
# define MOBJ_PLANE     3u
# define MOBJ_QUAD      4u

/*
** One GPU-friendly object descriptor.
** Offset map (all 4-byte aligned):
**   0   type           4B
**   4   center        12B   sphere/cylinder centre, plane/quad Q point
**   16  normal        12B   cylinder axis, plane/quad normal
**   28  u             12B   quad edge vector u (unused for others)
**   40  v             12B   quad edge vector v (unused for others)
**   52  w             12B   quad w = cross(u,v)/dot(n,n) (unused for others)
**   64  albedo        12B
**   76  emission      12B
**   88  radius         4B   sphere radius / cylinder radius
**   92  height         4B   cylinder full height
**   96  D              4B   plane/quad: dot(normal, Q)
**   100 is_emissive    4B
**   Total: 104B
*/
typedef struct s_metal_obj
{
	uint32_t	type;
	t_mfloat3	center;
	t_mfloat3	normal;
	t_mfloat3	u;
	t_mfloat3	v;
	t_mfloat3	w;
	t_mfloat3	albedo;
	t_mfloat3	emission;
	float		radius;
	float		height;
	float		D;
	uint32_t	is_emissive;
}	t_metal_obj;

#endif
