#include "cl_util.h"

int  PrintOpenCLInfo()
{
  cl_uint         numPlatforms;
  cl_platform_id  *platforms;
  cl_int          clError;

  clError = clGetPlatformIDs(0, NULL, &numPlatforms);
  CHECK_OCL_ERR("clGetPlatformIDs", clError);

  if (0 >= numPlatforms)
    return (0);

  printf("\nOpenCL platforms detected: %d", numPlatforms);

  platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
  CHECK_NULL(platforms);

  clError = clGetPlatformIDs(numPlatforms, platforms, NULL);
  CHECK_OCL_ERR("clGetPlatformIDs", clError);

  for (cl_uint i = 0; i < numPlatforms; i++)
  {
    cl_uint       numDevices = 0;
    cl_device_id  *devices = NULL;

    printf("\n%d. Platform: ", i + 1);
    PrintPlatformName(platforms[i]);

    clError = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    CHECK_OCL_ERR("clGetDeviceIDs", clError);

    printf("\n\tNumber of devices: %d", numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
    CHECK_NULL(devices);

    clError = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    CHECK_OCL_ERR("clGetDeviceIDs", clError);

    for (cl_uint j = 0; j < numDevices; j++)
    {
      printf("\n\t%d. Device: ", j + 1);
      PrintDeviceName(devices[j]);

      cl_device_type deviceType;
      clError = clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
      CHECK_OCL_ERR("clGetDeviceInfo", clError);

      switch (deviceType)
      {
        case CL_DEVICE_TYPE_CPU:
          printf("\n\t\tType: CPU");
          break ;
        case CL_DEVICE_TYPE_GPU:
          printf("\n\t\tType: GPU");
          break ;
        case CL_DEVICE_TYPE_ACCELERATOR: 
          printf("\n\t\tType: ACCELERATOR");
          break ;
        default: 
          printf("\n\t\tType: Unknown");
          break ;
      }
      
      cl_uint cuCnt = 0;
      clError = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cuCnt), &cuCnt, NULL);
      CHECK_OCL_ERR("clGetDeviceInfo", clError);
      printf("\n\t\tNumber of Compute Units: %d", cuCnt);

      cl_uint wItDim = 0;
      clError = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(wItDim), &wItDim, NULL);
      CHECK_OCL_ERR("clGetDeviceInfo", clError);
      printf("\n\t\tWork Item Dimensions: %d", wItDim);


      size_t workGroupSize = 0;
      clError = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workGroupSize), &workGroupSize, NULL);
      CHECK_OCL_ERR("clGetDeviceInfo", clError);
      printf("\n\t\tWork Group Size: %zu", workGroupSize);

    }
    if (devices)
      free(devices);
  }

  if (platforms)
    free(platforms);
  return numPlatforms;
}

void PrintPlatformName(cl_platform_id platform)
{
    cl_uint clError = 0;
    size_t nameSize = 0;
    char* platformName = NULL;
    
    clError = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &nameSize);
    CHECK_OCL_ERR("clGetPlatformInfo", clError);

    platformName = (char*)malloc(nameSize * sizeof(char));
    CHECK_NULL(platformName);

    clError = clGetPlatformInfo(platform, CL_PLATFORM_NAME, nameSize, platformName, NULL);
    CHECK_OCL_ERR("clGetPlatformInfo", clError);

    printf("%s", platformName);

    free(platformName);
}

///////////////////////////////////////////////////////////////////////////////
// Prints out the name of the input device.
void PrintDeviceName(cl_device_id device)
{
    cl_uint clError = 0;
    size_t nameSize = 0;
    char* deviceName = NULL;

    clError = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &nameSize);
    CHECK_OCL_ERR("clGetDeviceInfo", clError);

    deviceName = (char*)malloc(nameSize * sizeof(char));
    CHECK_NULL(deviceName);

    clError = clGetDeviceInfo(device, CL_DEVICE_NAME, nameSize, deviceName, NULL);
    CHECK_OCL_ERR("clGetDeviceInfo", clError);

    printf("%s", deviceName);

    free(deviceName);
}

void SelectOpenCLPlatformAndDevice(cl_platform_id *pPlatform, cl_device_id *pDevice)
{
  cl_uint numPlatforms;
  cl_uint numDevices;
  cl_uint clError;
  int     platformIndex;
  int     deviceIndex;
  cl_platform_id  *platforms;
  cl_device_id    *devices;

  CHECK_NULL(pPlatform);
  CHECK_NULL(pDevice);

  clError = clGetPlatformIDs(0, NULL, &numPlatforms);
  CHECK_OCL_ERR("clGetPlatformsIDs", clError);

  platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
  CHECK_NULL(platforms);

  clError = clGetPlatformIDs(numPlatforms, platforms, NULL);
  CHECK_OCL_ERR("clGetPlatformIDs", clError);

  printf("\n\nSelect platform to use [%d-%d]:", 1, numPlatforms);
  scanf("%d", &platformIndex);
  platformIndex--;

  *pPlatform = platforms[platformIndex];

  clError = clGetDeviceIDs(platforms[platformIndex], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
  CHECK_OCL_ERR("clGetDeviceIDs", clError);

  devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

  clError = clGetDeviceIDs(platforms[platformIndex], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

  printf("Select device to use [%d-%d]:", 1, numDevices);
  scanf("%d", &deviceIndex);
  deviceIndex--;

  *pDevice = devices[deviceIndex];
  if (platforms)
    free(platforms);
  if (devices) free(devices);
}

cl_context  CreateOpenCLContext(cl_platform_id platform, cl_device_id device)
{
  cl_int     clError;
  cl_context context;

  cl_context_properties contextProperties[] =
  {
    CL_CONTEXT_PLATFORM,
    (cl_context_properties)platform,
    0
  };
  context = clCreateContext(contextProperties, 1, &device, NULL, NULL, &clError);
  CHECK_OCL_ERR("clCreateContext", clError);

  return (context);
}

cl_command_queue  CreateOpenCLQueue(cl_device_id device, cl_context context)
{
  cl_int            clError;
  cl_command_queue  queue;

  queue = clCreateCommandQueueWithProperties(context, device, 0, &clError);
  CHECK_OCL_ERR("clCreateCommandQueue", clError);

  return (queue);
}

char  *LoadOpenCLSourceFromFile(char *filePath, size_t *pSourceLength)
{
  FILE  *fileHandle;
  char  *sourceCode;

  fileHandle = fopen(filePath, "rb");
  CHECK_NULL(fileHandle);
  fseek(fileHandle, 0, SEEK_END);

  *pSourceLength = ftell(fileHandle);
  sourceCode = (char *)malloc((*pSourceLength) + 1);
  CHECK_NULL(sourceCode);

  fseek(fileHandle, 0, SEEK_SET);
  fread(sourceCode, *pSourceLength, 1, fileHandle);
  sourceCode[(*pSourceLength)] = 0;
  /* *pSourceLength = (*pSourceLength) + 1; */

  fclose(fileHandle);
  return (sourceCode);
}

cl_program  CreateAndBuildProgram(cl_context context, cl_uint count, char **sourceCode, size_t *sourceCodeLength)
{
  cl_program    program;
  cl_int        clError;
  char          *buildLog;
  size_t        buildLogSize;
  cl_device_id  device;

  program = clCreateProgramWithSource(context, count, (const char**)(sourceCode), sourceCodeLength, &clError);
  CHECK_OCL_ERR("clCreateProgramWithSource", clError);

  clError = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, NULL);
  CHECK_OCL_ERR("clGetContextInfo", clError);

  char  *options = "-I. -I./kernels";
  clError = clBuildProgram(program, 1, &device, options, NULL, NULL);

  if (CL_SUCCESS != clError)
  {
    printf("\nOpenCL error %d at line %d in file %s\n", clError, __LINE__, __FILE__);
    
    buildLog = NULL;
    buildLogSize = 0;

    clError = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize);
    CHECK_OCL_ERR("clGetProgramBuildInfo", clError);

    if (buildLogSize)
    {
      buildLog = (char *)malloc(buildLogSize);
      if (buildLog)
      {
        clError = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
        CHECK_OCL_ERR("clGetProgramBuildInfo", clError);

        printf("\nOpenCL program build info: \n%s\n", buildLog);

        free(buildLog);
      }
    }
    exit(EXIT_FAILURE);
  }
  return (program);
}

cl_kernel CreateKernel(cl_program program, char *kernelName)
{
  cl_int    clError;
  cl_kernel kernel;

  kernel = clCreateKernel(program, kernelName, &clError);
  CHECK_OCL_ERR("clCreateKernel", clError);

  return (kernel);
}

cl_mem  CreateDeviceBuffer(cl_context context, size_t sizeInBytes)
{
  cl_int  clError;
  cl_mem  buffer;

  buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeInBytes, NULL, &clError);
  CHECK_OCL_ERR("clCreateBuffer", clError);

  return (buffer);
}

void  CopyHostToDevice(void *hostBuffer, cl_mem deviceBuffer, size_t sizeInBytes, cl_command_queue queue, cl_bool blocking)
{
  cl_int clError;
  
  clError = clEnqueueWriteBuffer(queue, deviceBuffer, blocking, 0, sizeInBytes, hostBuffer, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueReadBuffer", clError);
}

void CopyDeviceToHost(cl_mem deviceBuffer, void *hostBuffer, size_t sizeInBytes, cl_command_queue queue, cl_bool blocking)
{
  cl_int clError;

  clError = clEnqueueReadBuffer(queue, deviceBuffer, blocking, 0, sizeInBytes, hostBuffer, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueReadBuffer", clError);
}

/* void  SimpleFunctionOpenCL(cl_mem input1, cl_mem input2, cl_mem output, cl_command_queue queue, cl_kernel kernel) */
/* { */
/*   cl_int clError; */
/*   cl_uint workDim; */
/*   size_t  globalWorkSize[3]; */
/*   size_t  localWorkSize[3]; */
/**/
/*   clError = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)(&input1)); */
/*   CHECK_OCL_ERR("clSetKernelArg", clError); */
/**/
/*   clError = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)(&input2)); */
/*   CHECK_OCL_ERR("clSetKernelArg", clError); */
/**/
/*   clError = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)(&output)); */
/*   CHECK_OCL_ERR("clSetKernelArg", clError); */
/**/
/*   clError = clSetKernelArg(kernel, 3, sizeof(int), (void*)(&count)); */
/*   CHECK_OCL_ERR("clSetKernelArg", clError); */
/**/
/*   workDim = 1; */
/*   localWorkSize[0] = 256; */
/*   localWorkSize[1] = 0; */
/*   localWorkSize[2] = 0; */
/*   globalWorkSize[0] = ((count + localWorkSize[0] - 1) / localWorkSize[0]) * localWorkSize[0]; */
/*   globalWorkSize[1] = 0;  */
/*   globalWorkSize[2] = 0; */
/**/
/*   // Add the kernel to the queue */
/*   clError = clEnqueueNDRangeKernel(queue, kernel, workDim, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL); */
/*   CHECK_OCL_ERROR("clEnqueueNDRangeKernel", clError); */
/* } */

void  ReleaseMemObject(cl_mem *pBuffer)
{
  cl_int  clError;

  CHECK_NULL(pBuffer);

  if (*pBuffer)
  {
    clError = clReleaseMemObject(*pBuffer);
    CHECK_OCL_ERR("clReleaseMemObject", clError);

    *pBuffer = 0;
  }
}

void ReleaseProgram(cl_program *pProgram)
{
    cl_int clError;

    CHECK_NULL(pProgram);

    if (*pProgram)
    {
        clError = clReleaseProgram(*pProgram);
        CHECK_OCL_ERR("clReleaseProgram", clError);

        *pProgram = 0;
    }
}

void  ReleaseKernel(cl_kernel *pKernel)
{
  cl_int clError;

  CHECK_NULL(pKernel);
  if (*pKernel)
  {
    clError = clReleaseKernel(*pKernel);
    CHECK_OCL_ERR("clReleaseKernel", clError);
    *pKernel = 0;
  }
}

void ReleaseDeviceBuffer(cl_mem *pDeviceBuffer)
{
    cl_int clError;

    CHECK_NULL(pDeviceBuffer);

    if (*pDeviceBuffer)
    {
        clError = clReleaseMemObject(*pDeviceBuffer);
        CHECK_OCL_ERR("clReleaseMemObject", clError);

        *pDeviceBuffer = 0;
    }      
}

void ReleaseOpenCLQueue(cl_command_queue *pQueue)
{
    cl_int clError;

    CHECK_NULL(pQueue);

    if (*pQueue)
    {
        clError = clReleaseCommandQueue(*pQueue);
        CHECK_OCL_ERR("clReleaseCommandQueue", clError);

        *pQueue = 0;
    }
}

void ReleaseOpenCLContext(cl_context *pContext)
{
    cl_int clError;

    CHECK_NULL(pContext);

    if (*pContext)
    {
        clError = clReleaseContext(*pContext);
        CHECK_OCL_ERR("clReleaseContext", clError);

        *pContext = 0;
    }
}

