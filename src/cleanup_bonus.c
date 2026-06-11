#include "rt_bonus.h"

#define PATH_MAX 4096

void  SetupLocalCL(void)
{
  char cwd[PATH_MAX];

  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    char vendors_path[PATH_MAX + 64];
    char libs_path[PATH_MAX + 64];
    snprintf(vendors_path, sizeof(vendors_path), "%s/config/vendors", cwd);
    setenv("OCL_ICD_VENDORS", vendors_path, 1);
    printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
    snprintf(libs_path, sizeof(libs_path), "%s/libs", cwd);
    setenv("LD_LIBRARY_PATH", libs_path, 1);
  }
}

void clean_gpu(t_data *data)
{
  ReleaseMemObject(&(data->gpu.buffer));
  ReleaseMemObject(&(data->gpu.object_buffer));
  ReleaseMemObject(&(data->gpu.accum_buffer));
  ReleaseKernel(&(data->gpu.kernel));
  ReleaseProgram(&(data->gpu.program));
  ReleaseOpenCLQueue(&(data->gpu.queue));
  ReleaseOpenCLContext(&(data->gpu.context));
}

void cleanup(void *param)
{
  t_data *data = (t_data *)param;

  clean_gpu(data);
  free(data->objects);
  printf("Cleaning up and exiting...\n");
}
