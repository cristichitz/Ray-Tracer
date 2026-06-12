#include "rt_bonus.h"

// Uploads the already-assembled scene (filled by load_scene) as one buffer.
cl_int   init_objects(t_data *data)
{
  cl_int  clError;
  cl_int  count;

  data->gpu.object_buffer = clCreateBuffer(data->gpu.context,
      CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(t_object) * data->obj_count, data->objects, &clError);
  count = (cl_int)data->obj_count;
  clSetKernelArg(data->gpu.kernel, 1, sizeof(cl_mem), &data->gpu.object_buffer);
  clSetKernelArg(data->gpu.kernel, 2, sizeof(cl_int), &count);
  return (clError);
}

cl_int init_gpu(t_data *data)
{
  cl_int          clError;
  cl_platform_id  platform;
  cl_device_id    device;
  static const char *files[] = {
    "kernels/interval_bonus.c", "kernels/ray_bonus.c", "kernels/random_bonus.c",
    "kernels/hit_sphere_bonus.c", "kernels/hit_plane_bonus.c", "kernels/hit_quad_bonus.c",
    "kernels/hit_cylinder_bonus.c", "kernels/objects_bonus.c", "kernels/nee_bonus.c",
    "kernels/scatter_bonus.c", "kernels/camera_bonus.c", "kernels/color_bonus.c",
    "kernels/render_bonus.c"
  };
  int             n = (int)(sizeof(files) / sizeof(files[0]));
  char            *src[16];
  size_t          len[16];
  int             i;

  if (!PrintOpenCLInfo())
  {
    printf("\nNo OpenCL platform or device detected.");
    exit(EXIT_FAILURE);
  }
  SelectOpenCLPlatformAndDevice(&platform, &device);
  printf("\nUsing platform ");
  PrintPlatformName(platform);
  printf(" and device ");
  PrintDeviceName(device);
  printf("\n");
  data->gpu.context = CreateOpenCLContext(platform, device);
  data->gpu.queue = CreateOpenCLQueue(device, data->gpu.context);
  i = 0;
  while (i < n)
  {
    src[i] = LoadOpenCLSourceFromFile((char *)files[i], &len[i]);
    i++;
  }
  data->gpu.program = CreateAndBuildProgram(data->gpu.context, n, src, len);
  i = 0;
  while (i < n)
  {
    if (src[i])
      free(src[i]);
    i++;
  }
  data->gpu.kernel = CreateKernel(data->gpu.program, "render_kernel");
  data->gpu.buffer = CreateDeviceBuffer(data->gpu.context,
      data->frame.width * data->frame.height * sizeof(int));
  data->gpu.accum_buffer = CreateDeviceBuffer(data->gpu.context,
      data->frame.width * data->frame.height * sizeof(cl_float4));
  clError = clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
  CHECK_OCL_ERR("clSetKernelArg", clError);
  clError = clSetKernelArg(data->gpu.kernel, 4, sizeof(cl_mem), &data->gpu.accum_buffer);
  CHECK_OCL_ERR("clSetKernelArg", clError);
  clError = init_objects(data);
  return (clError);
}

void  render_frame(t_data *data)
{
  cl_int  clError;
  size_t  local_work[2];
  size_t  global_work[2];
  size_t  total_bytes;

  total_bytes = data->frame.width * data->frame.height * 4;
  local_work[0] = 16;
  local_work[1] = 16;
  global_work[0] = ((data->frame.width + 15) / 16) * 16;
  global_work[1] = ((data->frame.height + 15) / 16) * 16;
  clError = clSetKernelArg(data->gpu.kernel, 3, sizeof(t_image), &data->frame);
  CHECK_OCL_ERR("clSetKernelArg", clError);
  cl_int fidx = (cl_int)data->frame_index;
  clError = clSetKernelArg(data->gpu.kernel, 5, sizeof(cl_int), &fidx);
  CHECK_OCL_ERR("clSetKernelArg", clError);
  clError = clEnqueueWriteBuffer(data->gpu.queue, data->gpu.object_buffer, CL_TRUE, 0,
                                sizeof(t_object) * data->obj_count, data->objects, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueWriteBuffer", clError);
  cl_event ev;
  clError = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 2, NULL,
                                   global_work, local_work, 0, NULL, &ev);
  CHECK_OCL_ERR("clEnqueueNDRangeKernel", clError);
  clWaitForEvents(1, &ev);
  cl_ulong s, e;
  clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_START, sizeof(s), &s, NULL);
  clGetEventProfilingInfo(ev, CL_PROFILING_COMMAND_END, sizeof(e), &e, NULL);
  if (!data->render_mode)
    printf("kernel: %.2f ms\n", (e - s) / 1e6);
  clReleaseEvent(ev);
  clError = clFinish(data->gpu.queue);
  CHECK_OCL_ERR("clFinish", clError);
  clError = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0,
                    total_bytes, data->img->pixels, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueReadBuffer", clError);
}
