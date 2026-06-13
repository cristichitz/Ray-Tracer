/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_program_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:56:06 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:56:07 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cl_util_bonus.h"

char	*load_cl_source(const char *file_path, size_t *source_length)
{
	FILE	*file;
	char	*source;

	file = fopen(file_path, "rb");
	check_null(file);
	fseek(file, 0, SEEK_END);
	*source_length = ftell(file);
	source = (char *)malloc((*source_length) + 1);
	check_null(source);
	fseek(file, 0, SEEK_SET);
	fread(source, *source_length, 1, file);
	source[*source_length] = 0;
	fclose(file);
	return (source);
}

static void	print_build_log(cl_program program, cl_device_id device)
{
	cl_int	clerror;
	char	*log;
	size_t	log_size;

	log_size = 0;
	clerror = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			0, NULL, &log_size);
	check_ocl_err("clGetProgramBuildInfo", clerror);
	if (log_size == 0)
		return ;
	log = (char *)malloc(log_size);
	if (!log)
		return ;
	clerror = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size, log, NULL);
	check_ocl_err("clGetProgramBuildInfo", clerror);
	printf("\nOpenCL program build info: \n%s\n", log);
	free(log);
}

cl_program	create_build_program(cl_context context, cl_uint count,
		char **source, size_t *length)
{
	cl_program		program;
	cl_int			clerror;
	cl_device_id	device;

	program = clCreateProgramWithSource(context, count,
			(const char **)source, length, &clerror);
	check_ocl_err("clCreateProgramWithSource", clerror);
	clerror = clGetContextInfo(context, CL_CONTEXT_DEVICES,
			sizeof(cl_device_id), &device, NULL);
	check_ocl_err("clGetContextInfo", clerror);
	clerror = clBuildProgram(program, 1, &device,
			"-I. -I./bonus/kernels -cl-fast-relaxed-math", NULL, NULL);
	if (clerror != CL_SUCCESS)
	{
		printf("\nOpenCL build error %d in create_build_program\n", clerror);
		print_build_log(program, device);
		exit(EXIT_FAILURE);
	}
	return (program);
}

cl_mem	create_device_buffer(cl_context context, size_t size_in_bytes)
{
	cl_int	clerror;
	cl_mem	buffer;

	buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, size_in_bytes,
			NULL, &clerror);
	check_ocl_err("clCreateBuffer", clerror);
	return (buffer);
}
