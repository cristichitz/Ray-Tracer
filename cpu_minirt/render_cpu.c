#include "rt_cpu.h"

// --- Vector Math Helpers (Mimicking OpenCL built-ins) ---
t_vec3 vec_add(t_vec3 a, t_vec3 b) { return (t_vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
t_vec3 vec_sub(t_vec3 a, t_vec3 b) { return (t_vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
t_vec3 vec_scale(t_vec3 v, float s) { return (t_vec3){v.x * s, v.y * s, v.z * s}; }
float  vec_dot(t_vec3 a, t_vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

t_vec3 vec_norm(t_vec3 v) {
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return (t_vec3){v.x / len, v.y / len, v.z / len};
}

// --- Ray Color Logic (Identical to Kernel) ---
t_vec3 get_ray_color(t_vec3 origin, t_vec3 dir) {
    t_vec3 sphere_center = {0.0f, 0.0f, -1.0f};
    float radius = 0.5f;

    // Check Sphere Hit
    t_vec3 oc = vec_sub(origin, sphere_center);
    float a = vec_dot(dir, dir);
    float b = 2.0f * vec_dot(oc, dir);
    float c = vec_dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0.0f) {
        // Hit
        float t = (-b - sqrtf(discriminant)) / (2.0f * a);
        t_vec3 hit_point = vec_add(origin, vec_scale(dir, t));
        t_vec3 normal = vec_norm(vec_sub(hit_point, sphere_center));
        // 0.5 * (normal + 1.0)
        return vec_scale(vec_add(normal, (t_vec3){1.0f, 1.0f, 1.0f}), 0.5f);
    } else {
        // Background
        t_vec3 unit_dir = vec_norm(dir);
        float t = 0.5f * (unit_dir.y + 1.0f);
        t_vec3 white = {1.0f, 1.0f, 1.0f};
        t_vec3 blue = {0.5f, 0.7f, 1.0f};
        // (1.0 - t) * white + t * blue
        return vec_add(vec_scale(white, 1.0f - t), vec_scale(blue, t));
    }
}

// --- Main Render Loop (CPU) ---
int render_frame(t_data *data)
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    if (data->last_time.tv_sec == 0) {
        data->last_time = current_time;
        return (0);
    }

    long seconds = current_time.tv_sec - data->last_time.tv_sec;
    long micros = current_time.tv_usec - data->last_time.tv_usec;
    data->delta_time = seconds + micros * 1e-6;
    data->last_time = current_time; // Reset for next frame

    // 2. Print FPS Every Frame
    // Since CPU is slow, we don't need to skip frames. 
    // We print \r (carriage return) to overwrite the line instead of spamming new lines.
    printf("\rFrame Time: %.3fs | FPS: %.2f   ", data->delta_time, 1.0 / data->delta_time);
    fflush(stdout);

    // 2. Camera Setup
    float aspect_ratio = (float)data->width / (float)data->height;
    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    t_vec3 origin = {data->cam_x, data->cam_y, data->cam_z};
    t_vec3 horizontal = {viewport_width, 0.0f, 0.0f};
    t_vec3 vertical = {0.0f, viewport_height, 0.0f};
    
    // lower_left = origin - horizontal/2 - vertical/2 - (0,0,focal)
    t_vec3 lower_left_corner = vec_sub(origin, vec_scale(horizontal, 0.5f));
    lower_left_corner = vec_sub(lower_left_corner, vec_scale(vertical, 0.5f));
    lower_left_corner = vec_sub(lower_left_corner, (t_vec3){0.0f, 0.0f, focal_length});

    // 3. Pixel Loop (The heavy CPU work)
    int *buffer = (int *)data->addr;
    
    for (int y = 0; y < data->height; y++) {
        for (int x = 0; x < data->width; x++) {
            
            float u = (float)x / (float)(data->width - 1);
            float v = 1.0f - ((float)y / (float)(data->height - 1)); // Flip Y

            // direction = lower_left + u*hor + v*vert - origin
            t_vec3 dir = lower_left_corner;
            dir = vec_add(dir, vec_scale(horizontal, u));
            dir = vec_add(dir, vec_scale(vertical, v));
            dir = vec_sub(dir, origin);

            t_vec3 color = get_ray_color(origin, dir);

            // Convert to int color
            int ir = (int)(255.99f * color.x);
            int ig = (int)(255.99f * color.y);
            int ib = (int)(255.99f * color.z);

            buffer[y * data->width + x] = (ir << 16) | (ig << 8) | ib;
        }
    }

    // 4. Push to Window
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}

int close_app(t_data *data) {
    (void)data;
    exit(0);
    return (0);
}

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_ESC 65307

int key_hook(int keycode, t_data *data) {
    float speed = 0.5f;
    if (keycode == 53) // ESC
        close_app(data);
    if (keycode == KEY_W) data->cam_z -= speed; // Move Forward (into screen)
    if (keycode == KEY_S) data->cam_z += speed; // Move Back
    if (keycode == KEY_A) data->cam_x -= speed; // Move Left
    if (keycode == KEY_D) data->cam_x += speed; // Move Right
    
    return (0);
}