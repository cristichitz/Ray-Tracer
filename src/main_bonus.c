#include "rt_bonus.h"
#define PATH_MAX 4096

void  SetupLocalCL()
{
  char cwd[PATH_MAX];

  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    char vendors_path[PATH_MAX];
    snprintf(vendors_path, sizeof(vendors_path), "%s/config/vendors", cwd);
    setenv("OCL_ICD_VENDORS", vendors_path, 1);
    printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
  }
}

void clean_gpu(t_data *data)
{
  ReleaseMemObject(&(data->gpu.buffer));
  ReleaseMemObject(&(data->gpu.sphere_buffer));
  ReleaseKernel(&(data->gpu.kernel));
  ReleaseProgram(&(data->gpu.program));
  ReleaseOpenCLQueue(&(data->gpu.queue));
  ReleaseOpenCLContext(&(data->gpu.context));
}

void cleanup(void *param)
{
    t_data *data = (t_data *)param;
    clean_gpu(data); 
    printf("Cleaning up and exiting...\n");
}

float random_float(float min, float max)
{
  // random is a number between 0 and 1
  return (min + (max - min)*((rand() / ((double)RAND_MAX + 1))));
}

float degrees_to_radians(float degrees)
{
  return (degrees * M_PI / 180.0f);
}

void initialize(t_image *frame)
{
    frame->width = 1920;
    frame->height = (int)(frame->width / (16.0 / 9.0));
    frame->height = (frame->height < 1) ? 1 : frame->height;
    
    frame->origin = make_float3(0.0f, 0.0f, 0.0f);
    

    frame->aspect_ratio = (float)frame->width / (float)frame->height;
    frame->viewport_height = 2.0f;
    frame->viewport_width = frame->aspect_ratio * frame->viewport_height;
    frame->focal_length = 1.0f;

    // New. For Antialising
    frame->samples_per_pixel = 5;
    frame->pixel_samples_scale = 1.0f / frame->samples_per_pixel;
    frame->max_depth = 5;
    // until here

    // This was in rendering but they are alwas  the same
    frame->horizontal = make_float3(frame->viewport_width, 0.0f, 0.0f);
    frame->vertical = make_float3(0.0f, -frame->viewport_height, 0.0f);

    frame->pixel_delta_u =  scale(frame->horizontal, (float)1 / (float)frame->width);
    frame->pixel_delta_v = scale(frame->vertical, (float)1 / (float)frame->height);

    cl_float3 upper_left_corner = sub(frame->origin, scale(frame->horizontal, 0.5f));
    upper_left_corner = sub(upper_left_corner, scale(frame->vertical, 0.5f));
    upper_left_corner = sub(upper_left_corner, make_float3(0.0f, 0.0f, frame->focal_length));

    // corner + 0.5 * (pixel_delta_u + pixel_delta_v)
    frame->pixel00_loc = add(upper_left_corner, scale(add(frame->pixel_delta_u, frame->pixel_delta_v), 0.5f)); 
}

/* int create_objects(t_hittable_list *world) */
/* { */
/*   int status; */
/*   t_material    material_ground; */
/*   t_material    material_left; */
/*   t_material    material_center; */
/*   t_material    material_right; */
/**/
/*   material_ground = init_lambertian(make_float3(0.8f, 0.8f, 0.0f)); */
/*   material_left = init_metal(make_float3(0.8f, 0.8f, 0.8f)); */
/*   material_center = init_metal(make_float3(0.1f, 0.2f, 0.5f)); */
/*   material_right = init_metal(make_float3(0.8f, 0.6f, 0.2f)); */
/**/
/*   status = world->add(world, make_sphere(make_float3(0, -100.5, -1.0), 100.0, material_ground)); */
/*   status = world->add(world, make_sphere(make_float3(0, 0, -1.2), 0.5, material_left)); */
/*   status = world->add(world, make_sphere(make_float3(-1, 0, -1), 0.5, material_center)); */
/*   status = world->add(world, make_sphere(make_float3(1, 0, -1), 0.5, material_right)); */
/*   status = world->add(world, make_sphere(make_float3(1, 1, -1), 0.5, material_right)); */
/**/
/*   // MOVE THIS TO world->add  */
/*   world->obj_num = 5; */
/**/
/*   return (status); */
/* } */

t_sphere  make_sphere(cl_float3 center, float radius, t_material mat)
{
  t_sphere s;

  s.center = center;
  s.radius = radius;
  s.material = mat;
  return (s);
}

t_material  material_init(cl_float3 color, int type)
{
  t_material mat;
  mat.albedo = color;
  mat.type = type;

  return (mat);
}

cl_int   init_spheres(t_data *data)
{
  cl_int      clError;
  data->sphere_count = 4;
  data->spheres = (t_sphere*)malloc(sizeof(t_sphere) * data->sphere_count);


  t_material ground = material_init(make_float3(0.8f, 0.8f, 0.0f), 0);
  t_material left = material_init(make_float3(0.8f, 0.8f, 0.8f), 1);
  t_material center = material_init(make_float3(0.1f, 0.2f, 0.5f), 1);
  t_material right = material_init(make_float3(0.8f, 0.6f, 0.2f), 1);

  data->spheres[0] = make_sphere(make_float3(0, -100.5, -1.0), 100.0, ground);
  data->spheres[1] = make_sphere(make_float3(0, 0, -1.2), 0.5, center);
  data->spheres[2] = make_sphere(make_float3(-1, 0.0f, -1), 0.5, left);
  data->spheres[3] = make_sphere(make_float3(1, 0.0f, -1), 0.5, right);
  data->gpu.sphere_buffer = clCreateBuffer(data->gpu.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                          sizeof(t_sphere) * data->sphere_count, data->spheres, &clError);

  clSetKernelArg(data->gpu.kernel, 1, sizeof(cl_mem), &data->gpu.sphere_buffer);
  return (clError);
}

cl_int init_gpu(t_data *data)
{
  cl_int          clError;
  cl_platform_id  platform;
  cl_device_id    device;
  char*           sourceCode[3];
  size_t          sourceCodeLength[3];

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

  sourceCode[0] = LoadOpenCLSourceFromFile("kernels/internals.c", &(sourceCodeLength[0]));
  sourceCode[1] = LoadOpenCLSourceFromFile("kernels/random.c", &(sourceCodeLength[1]));
  sourceCode[2] = LoadOpenCLSourceFromFile("kernels/init_kernel.c", &(sourceCodeLength[2]));
  
  data->gpu.program = CreateAndBuildProgram(data->gpu.context, 3, sourceCode, sourceCodeLength);
  if (sourceCode[0])
    free(sourceCode[0]);
  if (sourceCode[1])
    free(sourceCode[1]);
  if (sourceCode[2])
    free(sourceCode[2]);

  data->gpu.kernel = CreateKernel(data->gpu.program, "render_kernel");
  data->gpu.buffer = CreateDeviceBuffer(data->gpu.context, data->frame.width * data->frame.height * sizeof(int));

  clError = clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
  CHECK_OCL_ERR("clSetKernelArg", clError);

  clError = init_spheres(data);
  return (clError);
}

void  render_frame(t_data *data)
{
cl_int  clError;
  size_t  local_work[2];
  size_t  global_work[2];
  size_t  total_bytes;

  total_bytes = data->frame.width *  data->frame.height * 4;

  local_work[0] = 16;
  local_work[1] = 16;
  global_work[0] = ((data->frame.width + 15) / 16) * 16;   // Global X
  global_work[1] = ((data->frame.height + 15) / 16) * 16;  // Global Y

  clError = clSetKernelArg(data->gpu.kernel, 2, sizeof(t_image), &data->frame);
  CHECK_OCL_ERR("clSetKernelArg", clError);

  // CopyHostToDevice
  clError = clEnqueueWriteBuffer(data->gpu.queue, data->gpu.sphere_buffer, CL_TRUE, 0,
                                sizeof(t_sphere) * data->sphere_count, data->spheres, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueWriteBuffer", clError);
  clError = clEnqueueNDRangeKernel(data->gpu.queue, data->gpu.kernel, 2, NULL,
                                   global_work, local_work, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueNDRangeKernel", clError);
  clError = clFinish(data->gpu.queue);
  CHECK_OCL_ERR("clFinish", clError);
  
  // CopyDeviceToHost
  // Read back into RAM
  clError = clEnqueueReadBuffer(data->gpu.queue, data->gpu.buffer, CL_TRUE, 0,
                    total_bytes, data->img->pixels, 0, NULL, NULL);
  CHECK_OCL_ERR("clEnqueueReadBuffer", clError);
}

void game_loop(void *param)
{
  t_data  *data = (t_data *)param;
  float   speed = 0.2f;

  if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
  {
    cleanup(data);
    mlx_close_window(data->mlx);
    return ;
  }

  if (mlx_is_key_down(data->mlx, MLX_KEY_W)) data->cam_z -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_S)) data->cam_z += speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_A)) data->cam_x -= speed;
  if (mlx_is_key_down(data->mlx, MLX_KEY_D)) data->cam_x += speed;

  data->frame.origin = make_float3(data->cam_x, data->cam_y, data->cam_z);
  cl_float3 upper_left_corner = sub(data->frame.origin, scale(data->frame.horizontal, 0.5f));
  upper_left_corner = sub(upper_left_corner, scale(data->frame.vertical, 0.5f));
  upper_left_corner = sub(upper_left_corner, make_float3(0.0f, 0.0f, data->frame.focal_length));

  // corner + 0.5 * (pixel_delta_u + pixel_delta_v)
  data->frame.pixel00_loc = add(upper_left_corner, scale(add(data->frame.pixel_delta_u, data->frame.pixel_delta_v), 0.5f));
  render_frame(data);
}

void  init_cam(t_data *data)
{
  data->cam_x = 0.0f;
  data->cam_y = 0.0f;
  data->cam_z = 0.0f;
}

int main(void)
{

  SetupLocalCL();

  //MINIRT STUFF

  t_data          data;
  t_image         frame;

  initialize(&frame);
  data.frame = frame;
  init_cam(&data);

  // Init MlX42
  data.mlx = mlx_init(frame.width, frame.height, "CPU RT", true);
  if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  data.img = mlx_new_image(data.mlx, frame.width, frame.height);
  if (!data.img) { mlx_close_window(data.mlx); puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1) {
    mlx_close_window(data.mlx);
    puts(mlx_strerror(mlx_errno));
    return(EXIT_FAILURE);
  }

  init_gpu(&data);

  mlx_loop_hook(data.mlx, game_loop, &data);
  mlx_close_hook(data.mlx, cleanup, &data);
  mlx_loop(data.mlx);

  mlx_terminate(data.mlx);
  return (EXIT_SUCCESS);
}
