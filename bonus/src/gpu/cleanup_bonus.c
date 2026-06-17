/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:31 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:21 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

#define PATH_MAX 4096

void	setup_local_cl(void)
{
	char	cwd[PATH_MAX];
	char	vendors_path[PATH_MAX + 64];
	char	libs_path[PATH_MAX + 64];

	if (access("/etc/OpenCL/vendors", F_OK) == 0)
		return ;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		snprintf(vendors_path, sizeof(vendors_path),
			"%s/bonus/config/vendors", cwd);
		setenv("OCL_ICD_VENDORS", vendors_path, 1);
		printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
		snprintf(libs_path, sizeof(libs_path), "%s/bonus/libs", cwd);
		setenv("LD_LIBRARY_PATH", libs_path, 1);
	}
}

void	clean_gpu(t_data *data)
{
	release_mem_object(&(data->gpu.buffer));
	release_mem_object(&(data->gpu.object_buffer));
	release_mem_object(&(data->gpu.accum_buffer));
	release_mem_object(&(data->gpu.node_buffer));
	release_mem_object(&(data->gpu.prim_buffer));
	release_kernel(&(data->gpu.kernel));
	release_program(&(data->gpu.program));
	release_cl_queue(&(data->gpu.queue));
	release_cl_context(&(data->gpu.context));
}

void	resize_gpu_buffers(t_data *data)
{
	cl_int	clerror;

	release_mem_object(&data->gpu.buffer);
	release_mem_object(&data->gpu.accum_buffer);
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
}

void	cleanup(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	clean_gpu(data);
	free(data->objects);
	data->objects = NULL;
	free(data->bvh.nodes);
	data->bvh.nodes = NULL;
	free(data->bvh.prim);
	data->bvh.prim = NULL;
	printf("Cleaning up and exiting...\n");
}
