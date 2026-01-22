#ifndef RT_CPU_H
#define RT_CPU_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "mlx.h" // Ensure minilibx is linked correctly

// Vector struct to replace OpenCL float3
typedef struct s_vec3 {
    float x, y, z;
} t_vec3;

typedef struct s_data {
    void    *mlx;
    void    *win;
    void    *img;
    char    *addr;
    int     bpp;
    int     line_len;
    int     endian;
    
    int     width;
    int     height;
    
    // Camera
    float   cam_x;
    float   cam_y;
    float   cam_z;

    // FPS / Time
    struct timeval last_time;
    double         delta_time;
    int            frame_count;
} t_data;

// Prototypes
int     render_frame(t_data *data);
int     close_app(t_data *data);
int     key_hook(int keycode, t_data *data);

#endif