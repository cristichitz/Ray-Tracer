#include "minirt.h"

void setup_local_opencl() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char vendors_path[PATH_MAX];
        snprintf(vendors_path, sizeof(vendors_path), "%s/config/vendors", cwd);
        
        setenv("OCL_ICD_VENDORS", vendors_path, 1);
        printf("OCL_ICD_VENDORS set to: %s\n", vendors_path);
    }
}

cl_float3 make_float3(float x, float y, float z)
{
    cl_float3 v;
    v.s[0] = x;
    v.s[1] = y;
    v.s[2] = z;
    v.s[3] = 0.0f;
    return (v);
}

void update_camera(t_camera *cam, int width, int height)
{
    float aspect_ratio = (float)width / (float)height;
    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    cl_float3 horizontal = make_float3(viewport_width, 0.0f, 0.0f); 
    cl_float3 vertical   = make_float3(0.0f, -viewport_height, 0.0f);

    cam->pixel_delta_u.x = horizontal.x / width;
    cam->pixel_delta_u.y = horizontal.y / width;
    cam->pixel_delta_u.z = horizontal.z / width;

    cam->pixel_delta_v.x = vertical.x / height;
    cam->pixel_delta_v.y = vertical.y / height;
    cam->pixel_delta_v.z = vertical.z / height;

    cl_float3 upper_left;
    upper_left.x = cam->origin.x - (horizontal.x / 2.0f) - (vertical.x / 2.0f) - 0.0f;
    upper_left.y = cam->origin.y - (horizontal.y / 2.0f) - (vertical.y / 2.0f) - 0.0f;
    upper_left.z = cam->origin.z - (horizontal.z / 2.0f) - (vertical.z / 2.0f) - focal_length;

    cam->pixel00_loc.x = upper_left.x + 0.5f * (cam->pixel_delta_u.x + cam->pixel_delta_v.x);
    cam->pixel00_loc.y = upper_left.y + 0.5f * (cam->pixel_delta_u.y + cam->pixel_delta_v.y);
    cam->pixel00_loc.z = upper_left.z + 0.5f * (cam->pixel_delta_u.z + cam->pixel_delta_v.z);
    // bool dir = true;
    // for (int i = 0; i < sphere_count; i++)
    // {
    //   if (spheres[i].center.x < 3.0f && dir)
    //   {
    //     dir = true;
    //     spheres[i].center.x += 0.1f;
    //   }
    //   else if ( spheres[i].center.x > -3.0f)
    //   {
    //     dir = false;
    //     spheres[i].center.x += 0.1f;
    //   }
    // }
}

void game_loop(void *param)
{
    t_data *data = (t_data *)param;
    bool moved = false;
    float speed = 0.5f;

    if (mlx_is_key_down(data->mlx, MLX_KEY_ESCAPE))
        mlx_close_window(data->mlx);

    if (mlx_is_key_down(data->mlx, MLX_KEY_W)) {
        data->camera.origin.z -= speed;
        moved = true;
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_S)) {
        data->camera.origin.z += speed;
        moved = true;
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_A)) {
        data->camera.origin.x -= speed;
        moved = true;
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_D)) {
        data->camera.origin.x += speed;
        moved = true;
    }

    if (moved)
    {
        update_camera(&data->camera, data->width, data->height);
    }

    printf("%f %f %f\n", data->camera.origin.x, data->camera.origin.y, data->camera.origin.z);

    render_frame(data);
}

void cleanup(void *param)
{
    t_data *data = (t_data *)param;
    clean_gpu(data);
    
    printf("Cleaning up and exiting...\n");
}

void  init_scene(t_data *data)
{
  data->sphere_count = 10;
  data->spheres = malloc(sizeof(t_sphere) * data->sphere_count);

  data->spheres[0].center = make_float3(1.0f, 0.0f, -2.0f);
  data->spheres[0].radius = 0.5f;

  data->spheres[1].center = make_float3(0.0f, 0.0f, -120.0f);
  data->spheres[1].radius = 100.0f;

  data->spheres[2].center = make_float3(0.0f, 0.0f, -1100.0f);
  data->spheres[2].radius = 1000.0f;

  data->spheres[3].center = make_float3(-1.0f, 0.0f, -2.0f);
  data->spheres[3].radius = 0.5f;

  data->spheres[4].center = make_float3(0.0f, -1.0f, -2.0f);
  data->spheres[4].radius = 0.5f;

  data->spheres[5].center = make_float3(0.0f, 1.0f, -2.0f);
  data->spheres[5].radius = 0.5f;

  data->spheres[6].center = make_float3(100.0f, 1.0f, -120.0f);
  data->spheres[6].radius = 100.0f;

  data->spheres[7].center = make_float3(-100.0f, 1.0f, -120.0f);
  data->spheres[7].radius = 100.0f;

  data->spheres[8].center = make_float3(0.0f, 100.0f, -120.0f);
  data->spheres[8].radius = 100.0f;

  data->spheres[9].center = make_float3(0.0f, -100.0f, -120.0f);
  data->spheres[9].radius = 100.0f;


  data->gpu.sphere_buffer = clCreateBuffer(data->gpu.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                          sizeof(t_sphere) * data->sphere_count, data->spheres, &data->err);
  clSetKernelArg(data->gpu.kernel, 0, sizeof(cl_mem), &data->gpu.buffer);
  clSetKernelArg(data->gpu.kernel, 1, sizeof(int), &data->width);
  clSetKernelArg(data->gpu.kernel, 2, sizeof(int), &data->height);
  clSetKernelArg(data->gpu.kernel, 3, sizeof(int), &data->sphere_count);
  clSetKernelArg(data->gpu.kernel, 4, sizeof(cl_mem), &data->gpu.sphere_buffer);

  data->camera.origin = (cl_float3){{0.0f, 0.0f, 0.0f}};
  update_camera(&data->camera, data->width, data->height);
}

int main(void)
{
  t_data  data;
  float   aspect_ratio;

  aspect_ratio = 16.0 / 9.0;
  data.width = 2560;
  data.height = (int)(data.width / aspect_ratio);
  if (data.height < 1)
    data.height = 1;
  data.frame_count = 0;
  gettimeofday(&data.last_time, NULL);


  setup_local_opencl();
  
  // 1. MLX Boilerplate
  printf("1. Initializing MLX42...\n");
  data.mlx = mlx_init(data.width, data.height, "GPU RT", true);
  if (!data.mlx) { puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  printf("3. Creating Image...\n");
  data.img = mlx_new_image(data.mlx, data.width, data.height);
  if (!data.img) { mlx_close_window(data.mlx); puts(mlx_strerror(mlx_errno)); return(EXIT_FAILURE); }

  if (mlx_image_to_window(data.mlx, data.img, 0, 0) == -1) {
    mlx_close_window(data.mlx);
    puts(mlx_strerror(mlx_errno));
    return(EXIT_FAILURE);
  }

  printf("4. Initializing GPU...\n");
  init_gpu(&data, "kernels/render.c");

  init_scene(&data);

  printf("4. Starting Loop...");
  mlx_loop_hook(data.mlx, game_loop, &data);
  mlx_close_hook(data.mlx, cleanup, &data);
  mlx_loop(data.mlx);

  mlx_terminate(data.mlx);

  return (EXIT_SUCCESS);
}