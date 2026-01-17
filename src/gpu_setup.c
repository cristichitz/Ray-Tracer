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
    data->gpu.queue = clCreateCommandQueueWithProperties(data->gpu.context, device, 0, &err);

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

    err = clBuildProgram(data->gpu.program, 1, &device, NULL, NULL, NULL);
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
        return; // Skip first frame to avoid huge delta_time
    }
    long seconds = current_time.tv_sec - data->last_time.tv_sec;
    long micros = current_time.tv_usec - data->last_time.tv_usec;
    data->delta_time = seconds + micros * 1e-6;

    data->last_time = current_time;
    data->frame_count++;
    if (data->delta_time > 0.001 && data->frame_count % 20 == 0) {
    printf("FPS: %.0f\n", 1.0 / data->delta_time);  
        fflush(stdout); // Force print immediately
    }

    cl_int err;
    size_t total_bytes = data->width * data->height * 4;


    // 3. Set Kernel Arguments
    clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
    clSetKernelArg(data->gpu.kernel, 1, sizeof(int), &data->width);
    clSetKernelArg(data->gpu.kernel, 2, sizeof(int), &data->height);

    clSetKernelArg(data->gpu.kernel, 3, sizeof(float), &data->cam_x);
    clSetKernelArg(data->gpu.kernel, 4, sizeof(float), &data->cam_y);
    clSetKernelArg(data->gpu.kernel, 5, sizeof(float), &data->cam_z);

    // 4. Enqueue Kernel (1D)
    size_t global_work = data->width * data->height;
    err = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 1, NULL, &global_work, NULL, 0, NULL, NULL);
    if (err) printf("Kernel Launch Error: %d\n", err);

    clFinish(data->gpu.queue);

    // 5. Read into MIDDLEMAN (Not MLX directly)
    err = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0, 
                        total_bytes, data->render_buffer, 0, NULL, NULL);
    
    if (err != CL_SUCCESS) {
        printf("ReadBuffer Error: %d\n", err);
        free(data->render_buffer);
        return;
    }
    memcpy(data->addr, data->render_buffer, total_bytes);
}

// void render_frame(t_data *data)
// {
//     cl_int err;

//     // 1. FILL WITH PURPLE (Debug check)
//     // We cast to int* to fill pixels easily
//     int *pixels = (int *)data->addr;
//     for (int i = 0; i < data->width * data->height; i++) {
//         pixels[i] = 0x00FF00FF; // Purple
//     }

//     // 2. Send Arguments
//     clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
//     clSetKernelArg(data->gpu.kernel, 1, sizeof(int), &data->width);
//     clSetKernelArg(data->gpu.kernel, 2, sizeof(int), &data->height);

//     // 3. Run Kernel (Try 2D as recommended, or stick to 1D if you prefer)
//     // Let's stick to your 1D for a moment to minimize variables
//     size_t global_work = data->width * data->height;
//     err = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 1, NULL, &global_work, NULL, 0, NULL, NULL);
//     if (err) printf("Kernel Error: %d\n", err);

//     // 4. FORCE WAIT (Crucial for testing)
//     clFinish(data->gpu.queue);

//     // 5. Read Back
//     // IMPORTANT: Check the error code here!
//     err = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0, 
//                         data->width * data->height * 4, data->addr, 0, NULL, NULL);
    
//     if (err != CL_SUCCESS) {
//         printf("CRITICAL ERROR: Read Buffer failed! Code: %d\n", err);
        
//         // Help debug common codes:
//         if (err == -38) printf("  -> CL_INVALID_MEM_OBJECT (Buffer is NULL/Garbage)\n");
//         if (err == -30) printf("  -> CL_INVALID_VALUE (Size or Pointers are wrong)\n");
//         if (err == -5)  printf("  -> CL_OUT_OF_RESOURCES (GPU crashed)\n");
//     }

//     // 6. Check Center Pixel
//     printf("Center Pixel after GPU: %X\n", pixels[(data->height/2) * data->width + (data->width/2)]);
// }

// void render_frame(t_data *data)
// {
//     cl_int err;
//     int test_pixel_before = 0x00FF00FF; // Purple
//     int test_pixel_after = 0;

//     printf("\n=== DEBUG: START FRAME ===\n");
//     printf("1. Pointer Check: data->addr is at %p\n", data->addr);
//     printf("2. Size Check: %d x %d (Total Bytes: %d)\n", data->width, data->height, data->width * data->height * 4);

//     // 1. Manually color the first pixel of MLX image to PURPLE
//     int *img_ptr = (int *)data->addr;
//     img_ptr[0] = test_pixel_before;

//     // 2. Run Kernel (Use your existing setup)
//     clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
//     clSetKernelArg(data->gpu.kernel, 1, sizeof(int), &data->width);
//     clSetKernelArg(data->gpu.kernel, 2, sizeof(int), &data->height);

//     size_t global_work = data->width * data->height;
//     err = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 1, NULL, &global_work, NULL, 0, NULL, NULL);
//     if (err) printf("!! Kernel Launch Error: %d\n", err);

//     clFinish(data->gpu.queue); // Wait for GPU to finish

//     // 3. TEST READ: Read JUST THE FIRST PIXEL into a local variable
//     // We do not trust data->addr right now. Let's read into 'test_pixel_after'.
//     err = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0, 
//                               sizeof(int), &test_pixel_after, 0, NULL, NULL);
    
//     if (err) printf("!! ReadBuffer Error: %d\n", err);

//     printf("3. GPU Kernel Result:\n");
//     printf("   - We put PURPLE (00FF00FF) on CPU.\n");
//     printf("   - GPU should have written WHITE (FFFFFFFF).\n");
//     printf("   - READ RESULT: %X\n", test_pixel_after);

//     if (test_pixel_after == 0) {
//         printf("   -> CONCLUSION: The GPU is writing ZEROS or not running.\n");
//     } else if (test_pixel_after == test_pixel_before) {
//         printf("   -> CONCLUSION: The ReadBuffer failed silently (Data didn't change).\n");
//     } else {
//         printf("   -> CONCLUSION: GPU works! The issue is data->addr pointer.\n");
//         // If this works, commit the full read
//         clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0, 
//                             data->width * data->height * 4, data->addr, 0, NULL, NULL);
//     }
//     printf("=== DEBUG: END FRAME ===\n");
// }

void clean_gpu(t_data *data)
{
    clReleaseMemObject(data->gpu.buffer);
    clReleaseKernel(data->gpu.kernel);
    clReleaseProgram(data->gpu.program);
    clReleaseCommandQueue(data->gpu.queue);
    clReleaseContext(data->gpu.context);
}