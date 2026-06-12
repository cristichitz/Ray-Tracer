/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_release_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:56:03 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:56:04 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cl_util_bonus.h"

void	release_mem_object(cl_mem *buffer)
{
	cl_int	clerror;

	check_null(buffer);
	if (*buffer)
	{
		clerror = clReleaseMemObject(*buffer);
		check_ocl_err("clReleaseMemObject", clerror);
		*buffer = 0;
	}
}

void	release_program(cl_program *program)
{
	cl_int	clerror;

	check_null(program);
	if (*program)
	{
		clerror = clReleaseProgram(*program);
		check_ocl_err("clReleaseProgram", clerror);
		*program = 0;
	}
}

void	release_kernel(cl_kernel *kernel)
{
	cl_int	clerror;

	check_null(kernel);
	if (*kernel)
	{
		clerror = clReleaseKernel(*kernel);
		check_ocl_err("clReleaseKernel", clerror);
		*kernel = 0;
	}
}

void	release_cl_queue(cl_command_queue *queue)
{
	cl_int	clerror;

	check_null(queue);
	if (*queue)
	{
		clerror = clReleaseCommandQueue(*queue);
		check_ocl_err("clReleaseCommandQueue", clerror);
		*queue = 0;
	}
}

void	release_cl_context(cl_context *context)
{
	cl_int	clerror;

	check_null(context);
	if (*context)
	{
		clerror = clReleaseContext(*context);
		check_ocl_err("clReleaseContext", clerror);
		*context = 0;
	}
}
