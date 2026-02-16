#ifndef RT_CPU_H
#define RT_CPU_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "MLX42/MLX42.h"

// Vector struct to replace OpenCL float3
typedef struct s_vec3 {
    float x;
    float y;
    float z;
} t_vec3;

typedef struct s_data {
    void    *mlx;
    void    *img;
    
    uint32_t     width;
    uint32_t    height;
    
    // Camera
    float   cam_x;
    float   cam_y;
    float   cam_z;
} t_data;

// Prototypes
int     render_frame(t_data *data);
int     close_app(t_data *data);
int     key_hook(int keycode, t_data *data);

#endif
