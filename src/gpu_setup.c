#include "minirt.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static char *read_kernel_source(char *filename)
{
    int         fd;
    struct stat st;
    char        *buf;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error opening kernel file");
        exit(1);
    }

    fstat(fd, &st);
    buf = malloc(st.st_size + 1);
    read(fd, buf, st.st_size);
    buf[st.st_size] = '\0';
    close(fd);
    return (buf);
}

void    init_gpu(t_data *data, char *kernel_file)
{
    cl_int          err;
    cl_platform_id  platform;
    cl_device_id    device;

    cl_uint num_platforms;
    
    // 1. Ask: "Do you have ANY platforms?"
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    
    if (err != CL_SUCCESS) {
        printf("Error: clGetPlatformIDs failed with code %d\n", err);
        if (err == -1001) printf("  -> Code -1001 means CL_PLATFORM_NOT_FOUND_KHR (No drivers found)\n");
        exit(1);
    }
    
    if (num_platforms == 0) {
        printf("Error: No OpenCL platforms found on this system.\n");
        exit(1);
    }

    printf("Success: Found %d platform(s)!\n", num_platforms);

    // 2. Now actually get the ID
    err = clGetPlatformIDs(1, &platform, NULL);
    if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL) != CL_SUCCESS) {
        printf("Error: No GPU device found.\n");
        exit(1);
    }
    data->gpu.context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_queue_properties properties[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    data->gpu.queue = clCreateCommandQueueWithProperties(data->gpu.context, device, properties, &err);

    if (err != CL_SUCCESS) {
        printf("Error: Failed to create Command Queue! %d\n", err);
        exit(1);
    }

    printf("We are here\n");
    // 2. Compile Kernel
    char *src = read_kernel_source(kernel_file);
    data->gpu.program = clCreateProgramWithSource(data->gpu.context, 1, (const char **)&src, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error creating program\n");
        exit(1);
    }
    char *options = "-I. -I./include";

    err = clBuildProgram(data->gpu.program, 1, &device, options, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char    buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(data->gpu.program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    }
    free(src);

    data->gpu.kernel = clCreateKernel(data->gpu.program, "render_kernel", &err);
    if (!data->gpu.kernel || err != CL_SUCCESS)
    {
        printf("Error: Failed to  create kernel! code: %d\n", err);
        exit(1);
    }

    size_t size = data->width * data->height * 4;
    data->gpu.buffer = clCreateBuffer(data->gpu.context, CL_MEM_WRITE_ONLY, size, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error: Failed to create Buffer! Code: %d\n", err);
        exit(1);
    }
}

#include <string.h> // Required for memcpy

void render_frame(t_data *data)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    if (data->last_time.tv_sec == 0) {
        data->last_time = current_time;
        return ;
    }

    long seconds = current_time.tv_sec - data->last_time.tv_sec;
    long micros = current_time.tv_usec - data->last_time.tv_usec;
    data->delta_time = seconds + micros * 1e-6;
    data->last_time = current_time;

    printf("\rFrame Time: %.3fs | FPS: %.2f   ", data->delta_time, 1.0 / data->delta_time);
    fflush(stdout);

    cl_int err;
    size_t total_bytes = data->width * data->height * 4;

    err = clSetKernelArg(data->gpu.kernel, 5, sizeof(t_camera), &data->camera);
    if (err != CL_SUCCESS) printf("Arg Error: %d\n", err);


    // BLOCK SIZE: 16x16 = 256 threads per group.
    // This is a standard "sweet spot" for most GPUs.
    size_t local_work[2] = {16, 16};
    size_t global_work[2] = {
        ((data->width + 15) / 16) * 16,  // Global X
        ((data->height + 15) / 16) * 16  // Global Y
    };

    // 3. Enqueue 2D Kernel
    // Note the '2' in the 3rd argument (dimensions)
    // Note we pass 'local_work' instead of NULL
    cl_event event;
    err = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 2, NULL, 
                                 global_work, local_work, 0, NULL, &event);                        
    if (err) printf("Kernel Launch Error: %d\n", err);

    clWaitForEvents(1, &event);

    cl_ulong start, end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);

    double gpu_time_ms = (double)(end - start) / 1000000.0;
    printf("GPU Kernel TIme: %.3f ms\n", gpu_time_ms);

    clFinish(data->gpu.queue);

    err = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0, 
                        total_bytes, data->img->pixels, 0, NULL, NULL);
    
    if (err != CL_SUCCESS) {
        printf("ReadBuffer Error: %d\n", err);
        return;
    }
}

void clean_gpu(t_data *data)
{
    clReleaseMemObject(data->gpu.buffer);
    clReleaseKernel(data->gpu.kernel);
    clReleaseProgram(data->gpu.program);
    clReleaseCommandQueue(data->gpu.queue);
    clReleaseContext(data->gpu.context);
}