/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_util_bonus.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 20:14:59 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 20:15:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CL_UTIL_BONUS_H
# define CL_UTIL_BONUS_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# ifdef METAL_GPU

#  include "cl_compat_bonus.h"

# else /* OpenCL path */

#  ifdef __APPLE__
#   include <OpenCL/opencl.h>
#  else
#   include <CL/cl.h>
#  endif

void				check_ocl_err(const char *function_name, cl_int err);
int					print_opencl_info(void);
void				print_platform_name(cl_platform_id platform);
void				print_device_name(cl_device_id device);
void				print_device_info(cl_device_id device, cl_uint index);
void				select_platform_device(cl_platform_id *platform,
						cl_device_id *device);
cl_context			create_cl_context(cl_platform_id platform,
						cl_device_id device);
cl_command_queue	create_cl_queue(cl_device_id device, cl_context context);
cl_kernel			create_cl_kernel(cl_program program, char *kernel_name);
cl_mem				create_device_buffer(cl_context context,
						size_t size_in_bytes);
char				*load_cl_source(const char *file_path,
						size_t *source_length);
cl_program			create_build_program(cl_context context, cl_uint count,
						char **source, size_t *length);
void				release_mem_object(cl_mem *buffer);
void				release_program(cl_program *program);
void				release_kernel(cl_kernel *kernel);
void				release_cl_queue(cl_command_queue *queue);
void				release_cl_context(cl_context *context);
void				setup_local_cl(void);

# endif /* METAL_GPU */

/* Available for both Metal and OpenCL paths */
void				check_null(const void *ptr);

#endif
