/* ************************************************************************** */
/*                                                                            */
/*   cl_compat_bonus.h                                                        */
/*                                                                            */
/*   Drop-in replacement for CL/cl_platform.h vector types when building      */
/*   without OpenCL (e.g. Metal GPU backend on macOS).                        */
/*   Layout is identical to the real cl_float3 / cl_float4:                   */
/*     • 16-byte aligned union with .x .y .z .w members                       */
/*     • cl_float3 == cl_float4 (per OpenCL spec section 6.1.5)               */
/*                                                                            */
/* ************************************************************************** */

#ifndef CL_COMPAT_BONUS_H
# define CL_COMPAT_BONUS_H

# include <stdint.h>

typedef int32_t		cl_int;
typedef uint32_t	cl_uint;
typedef float		cl_float;

typedef union __attribute__((aligned(16)))
{
	float	s[4];
	struct
	{
		float	x;
		float	y;
		float	z;
		float	w;
	};
}				cl_float4;

/* cl_float3 is identical in size, alignment and behavior to cl_float4. */
typedef cl_float4	cl_float3;

/* OpenCL math constants used by host code */
# ifndef CL_M_PI
#  define CL_M_PI 3.14159265358979323846
# endif
# ifndef CL_M_PI_F
#  define CL_M_PI_F 3.14159265358979f
# endif

#endif
