/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_info_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:56:09 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:56:10 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cl_util_bonus.h"

void	print_platform_name(cl_platform_id platform)
{
	cl_int	clerror;
	size_t	size;
	char	*name;

	clerror = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &size);
	check_ocl_err("clGetPlatformInfo", clerror);
	name = (char *)malloc(size);
	check_null(name);
	clerror = clGetPlatformInfo(platform, CL_PLATFORM_NAME, size, name, NULL);
	check_ocl_err("clGetPlatformInfo", clerror);
	printf("%s", name);
	free(name);
}

void	print_device_name(cl_device_id device)
{
	cl_int	clerror;
	size_t	size;
	char	*name;

	clerror = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &size);
	check_ocl_err("clGetDeviceInfo", clerror);
	name = (char *)malloc(size);
	check_null(name);
	clerror = clGetDeviceInfo(device, CL_DEVICE_NAME, size, name, NULL);
	check_ocl_err("clGetDeviceInfo", clerror);
	printf("%s", name);
	free(name);
}

static void	print_device_type(cl_device_id device)
{
	cl_int			clerror;
	cl_device_type	type;

	clerror = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type),
			&type, NULL);
	check_ocl_err("clGetDeviceInfo", clerror);
	if (type == CL_DEVICE_TYPE_CPU)
		printf("\n\t\tType: CPU");
	else if (type == CL_DEVICE_TYPE_GPU)
		printf("\n\t\tType: GPU");
	else if (type == CL_DEVICE_TYPE_ACCELERATOR)
		printf("\n\t\tType: ACCELERATOR");
	else
		printf("\n\t\tType: Unknown");
}

static void	print_device_caps(cl_device_id device)
{
	cl_int	clerror;
	cl_uint	cu_count;
	cl_uint	dim;
	size_t	wg_size;

	clerror = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS,
			sizeof(cu_count), &cu_count, NULL);
	check_ocl_err("clGetDeviceInfo", clerror);
	printf("\n\t\tNumber of Compute Units: %d", cu_count);
	clerror = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
			sizeof(dim), &dim, NULL);
	check_ocl_err("clGetDeviceInfo", clerror);
	printf("\n\t\tWork Item Dimensions: %d", dim);
	clerror = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
			sizeof(wg_size), &wg_size, NULL);
	check_ocl_err("clGetDeviceInfo", clerror);
	printf("\n\t\tWork Group Size: %zu", wg_size);
}

void	print_device_info(cl_device_id device, cl_uint index)
{
	printf("\n\t%d. Device: ", index + 1);
	print_device_name(device);
	print_device_type(device);
	print_device_caps(device);
}
