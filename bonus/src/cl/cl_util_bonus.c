/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_util_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:57 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:55:57 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cl_util_bonus.h"

void	check_ocl_err(const char *function_name, cl_int err)
{
	if (err != CL_SUCCESS)
	{
		printf("OpenCL error %d in %s\n", err, function_name);
		exit(EXIT_FAILURE);
	}
}

void	check_null(const void *ptr)
{
	if (ptr == NULL)
	{
		printf("NULL pointer error\n");
		exit(EXIT_FAILURE);
	}
}

cl_context	create_cl_context(cl_platform_id platform, cl_device_id device)
{
	cl_int					clerror;
	cl_context				context;
	cl_context_properties	props[3];

	props[0] = CL_CONTEXT_PLATFORM;
	props[1] = (cl_context_properties)platform;
	props[2] = 0;
	context = clCreateContext(props, 1, &device, NULL, NULL, &clerror);
	check_ocl_err("clCreateContext", clerror);
	return (context);
}

cl_command_queue	create_cl_queue(cl_device_id device, cl_context context)
{
	cl_int				clerror;
	cl_command_queue	queue;
	cl_queue_properties	props[3];

	props[0] = CL_QUEUE_PROPERTIES;
	props[1] = CL_QUEUE_PROFILING_ENABLE;
	props[2] = 0;
	queue = clCreateCommandQueueWithProperties(context, device, props,
			&clerror);
	check_ocl_err("clCreateCommandQueue", clerror);
	return (queue);
}

cl_kernel	create_cl_kernel(cl_program program, char *kernel_name)
{
	cl_int		clerror;
	cl_kernel	kernel;

	kernel = clCreateKernel(program, kernel_name, &clerror);
	check_ocl_err("clCreateKernel", clerror);
	return (kernel);
}
