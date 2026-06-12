/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_gpu_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:25 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:25 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"

static void	set_frame_args(t_data *data)
{
	cl_int	clerror;
	cl_int	fidx;

	fidx = (cl_int)data->frame_index;
	clerror = clSetKernelArg(data->gpu.kernel, 3, sizeof(t_image),
			&data->frame);
	check_ocl_err("clSetKernelArg", clerror);
	clerror = clSetKernelArg(data->gpu.kernel, 5, sizeof(cl_int), &fidx);
	check_ocl_err("clSetKernelArg", clerror);
	clerror = clEnqueueWriteBuffer(data->gpu.queue, data->gpu.object_buffer,
			CL_TRUE, 0, sizeof(t_object) * data->obj_count, data->objects,
			0, NULL, NULL);
	check_ocl_err("clEnqueueWriteBuffer", clerror);
}

static void	profile_kernel(cl_event ev, int render_mode)
{
	cl_ulong	start;
	cl_ulong	end;

	clWaitForEvents(1, &ev);
	clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_START,
		sizeof(start), &start, NULL);
	clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_END,
		sizeof(end), &end, NULL);
	if (!render_mode)
		printf("kernel: %.2f ms\n", (end - start) / 1e6);
	clReleaseEvent(ev);
}

void	render_frame(t_data *data)
{
	cl_int		clerror;
	size_t		local_work[2];
	size_t		global_work[2];
	cl_event	ev;

	local_work[0] = 16;
	local_work[1] = 16;
	global_work[0] = ((data->frame.width + 15) / 16) * 16;
	global_work[1] = ((data->frame.height + 15) / 16) * 16;
	set_frame_args(data);
	clerror = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 2,
			NULL, global_work, local_work, 0, NULL, &ev);
	check_ocl_err("clEnqueueNDRangeKernel", clerror);
	profile_kernel(ev, data->render_mode);
	clerror = clFinish(data->gpu.queue);
	check_ocl_err("clFinish", clerror);
	clerror = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE,
			0, data->frame.width * data->frame.height * 4,
			data->img->pixels, 0, NULL, NULL);
	check_ocl_err("clEnqueueReadBuffer", clerror);
}
