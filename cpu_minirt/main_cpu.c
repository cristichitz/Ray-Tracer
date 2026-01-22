#include "rt_cpu.h"

int main(void)
{
    t_data  data;

    data.width = 3840;
    data.height = (int)(data.width / (16.0 / 9.0));
    data.height = (data.height < 1) ? 1 : data.height;

    data.cam_x = 0.0f;
    data.cam_y = 0.0f;
    data.cam_z = 0.0f;
    
    data.frame_count = 0;
    gettimeofday(&data.last_time, NULL);

    // 1. Init Minilibx
    printf("--- CPU RAYTRACER ---\n");
    printf("1. Initializing Minilibx...\n");
    data.mlx = mlx_init();
    if (!data.mlx) return (1);

    printf("2. Creating Window...\n");
    data.win = mlx_new_window(data.mlx, data.width, data.height, "CPU RT");

    printf("3. Creating Image...\n");
    data.img = mlx_new_image(data.mlx, data.width, data.height);
    data.addr = mlx_get_data_addr(data.img, &data.bpp, &data.line_len, &data.endian);

    printf("4. Starting Loop (CPU Rendering)...\n");
    
    // Hooks
    mlx_hook(data.win, 17, 0, close_app, &data); // X button
    mlx_key_hook(data.win, key_hook, &data);
    mlx_loop_hook(data.mlx, render_frame, &data);
    
    mlx_loop(data.mlx);

    return (0);
}