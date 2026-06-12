/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_select_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:55:59 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:56:00 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cl_util_bonus.h"

static void	print_platform_devices(cl_platform_id platform)
{
	cl_uint			num;
	cl_device_id	*devices;
	cl_int			clerror;
	cl_uint			i;

	num = 0;
	clerror = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num);
	check_ocl_err("clGetDeviceIDs", clerror);
	printf("\n\tNumber of devices: %d", num);
	devices = (cl_device_id *)malloc(num * sizeof(cl_device_id));
	check_null(devices);
	clerror = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num,
			devices, NULL);
	check_ocl_err("clGetDeviceIDs", clerror);
	i = 0;
	while (i < num)
	{
		print_device_info(devices[i], i);
		i++;
	}
	free(devices);
}

int	print_opencl_info(void)
{
	cl_uint			num;
	cl_platform_id	*platforms;
	cl_int			clerror;
	cl_uint			i;

	clerror = clGetPlatformIDs(0, NULL, &num);
	check_ocl_err("clGetPlatformIDs", clerror);
	if (num == 0)
		return (0);
	printf("\nOpenCL platforms detected: %d", num);
	platforms = (cl_platform_id *)malloc(num * sizeof(cl_platform_id));
	check_null(platforms);
	clerror = clGetPlatformIDs(num, platforms, NULL);
	check_ocl_err("clGetPlatformIDs", clerror);
	i = 0;
	while (i < num)
	{
		printf("\n%d. Platform: ", i + 1);
		print_platform_name(platforms[i]);
		print_platform_devices(platforms[i]);
		i++;
	}
	free(platforms);
	return ((int)num);
}

static cl_platform_id	ask_platform(void)
{
	cl_uint			num;
	cl_int			clerror;
	int				idx;
	cl_platform_id	*platforms;
	cl_platform_id	chosen;

	clerror = clGetPlatformIDs(0, NULL, &num);
	check_ocl_err("clGetPlatformIDs", clerror);
	platforms = (cl_platform_id *)malloc(num * sizeof(cl_platform_id));
	check_null(platforms);
	clerror = clGetPlatformIDs(num, platforms, NULL);
	check_ocl_err("clGetPlatformIDs", clerror);
	printf("\n\nSelect platform to use [%d-%d]:", 1, num);
	scanf("%d", &idx);
	chosen = platforms[idx - 1];
	free(platforms);
	return (chosen);
}

static cl_device_id	ask_device(cl_platform_id platform)
{
	cl_uint			num;
	cl_int			clerror;
	int				idx;
	cl_device_id	*devices;
	cl_device_id	chosen;

	clerror = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num);
	check_ocl_err("clGetDeviceIDs", clerror);
	devices = (cl_device_id *)malloc(num * sizeof(cl_device_id));
	check_null(devices);
	clerror = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num,
			devices, NULL);
	check_ocl_err("clGetDeviceIDs", clerror);
	printf("Select device to use [%d-%d]:", 1, num);
	scanf("%d", &idx);
	chosen = devices[idx - 1];
	free(devices);
	return (chosen);
}

void	select_platform_device(cl_platform_id *platform, cl_device_id *device)
{
	check_null(platform);
	check_null(device);
	*platform = ask_platform();
	*device = ask_device(*platform);
}
