#ifndef MINIRT_H
# define MINIRT_H

# include "../include/CL/cl.h"  // or just <CL/cl.h> if you add the -I flag
#include "MLX42/MLX42.h"

#include "vec3.h"
#include "structs.h"

#include "stdio.h"
#include "math.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>



typedef struct s_gpu {
  cl_context        context;
  cl_command_queue  queue;
  cl_program        program;
  cl_kernel         kernel;
  cl_mem            buffer;
  cl_mem            sphere_buffer;
} t_gpu;

typedef struct s_data {
  mlx_t         *mlx;
  mlx_image_t   *img;

  int           width;
  int           height;

  t_gpu         gpu;
  cl_int        err;

  //FPS
  struct timeval  last_time;  
  int             frame_count;
  double          delta_time; // Time taken for last frame

  // Scene objects
  t_camera      camera;
  t_sphere      *spheres;
  int           sphere_count;
} t_data;

void  init_scene(t_data *data); // New helper to create spheres
void  update_camera_vectors(t_data *data); // Helper to calc vectors

void  init_gpu(t_data *data, char *kernel_file);
void  render_frame(t_data *data);
void  clean_gpu(t_data *data);

void  game_loop(void *param);

#endif