/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_gpu_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:28 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:28 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

#define KERNEL_COUNT 13

/*
** Uploads the already-assembled scene (filled by load_scene) as one buffer.
*/
cl_int	init_objects(t_data *data)
{
	cl_int	clerror;
	cl_int	count;

	data->gpu.object_buffer = clCreateBuffer(data->gpu.context,
			CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			sizeof(t_object) * data->obj_count, data->objects, &clerror);
	count = (cl_int)data->obj_count;
	clSetKernelArg(data->gpu.kernel, 1, sizeof(cl_mem),
		&data->gpu.object_buffer);
	clSetKernelArg(data->gpu.kernel, 2, sizeof(cl_int), &count);
	return (clerror);
}

static void	build_kernels(t_data *data)
{
	static const char	*files[] = {"bonus/kernels/interval_bonus.c",
		"bonus/kernels/ray_bonus.c", "bonus/kernels/random_bonus.c",
		"bonus/kernels/hit_sphere_bonus.c", "bonus/kernels/hit_plane_bonus.c",
		"bonus/kernels/hit_quad_bonus.c", "bonus/kernels/hit_cylinder_bonus.c",
		"bonus/kernels/objects_bonus.c", "bonus/kernels/nee_bonus.c",
		"bonus/kernels/scatter_bonus.c", "bonus/kernels/camera_bonus.c",
		"bonus/kernels/color_bonus.c", "bonus/kernels/render_bonus.c"};
	char				*src[KERNEL_COUNT];
	size_t				len[KERNEL_COUNT];
	int					i;

	i = 0;
	while (i < KERNEL_COUNT)
	{
		src[i] = load_cl_source(files[i], &len[i]);
		i++;
	}
	data->gpu.program = create_build_program(data->gpu.context,
			KERNEL_COUNT, src, len);
	i = 0;
	while (i < KERNEL_COUNT)
	{
		free(src[i]);
		i++;
	}
}

static cl_int	init_buffers(t_data *data)
{
	cl_int	clerror;

	data->gpu.buffer = create_device_buffer(data->gpu.context,
			data->frame.width * data->frame.height * sizeof(int));
	data->gpu.accum_buffer = create_device_buffer(data->gpu.context,
			data->frame.width * data->frame.height * sizeof(cl_float4));
	clerror = clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem),
			&data->gpu.buffer);
	check_ocl_err("clSetKernelArg", clerror);
	clerror = clSetKernelArg(data->gpu.kernel, 4, sizeof(cl_mem),
			&data->gpu.accum_buffer);
	check_ocl_err("clSetKernelArg", clerror);
	return (init_objects(data));
}

cl_int	init_gpu(t_data *data)
{
	cl_platform_id	platform;
	cl_device_id	device;

	if (!print_opencl_info())
	{
		printf("\nNo OpenCL platform or device detected.");
		exit(EXIT_FAILURE);
	}
	select_platform_device(&platform, &device);
	printf("\nUsing platform ");
	print_platform_name(platform);
	printf(" and device ");
	print_device_name(device);
	printf("\n");
	data->gpu.context = create_cl_context(platform, device);
	data->gpu.queue = create_cl_queue(device, data->gpu.context);
	build_kernels(data);
	data->gpu.kernel = create_cl_kernel(data->gpu.program, "render_kernel");
	return (init_buffers(data));
}
