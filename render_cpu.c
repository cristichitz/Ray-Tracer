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

    for (uint32_t y = 0; y < data->height; y++) {
        for (uint32_t x = 0; x < data->width; x++) {
            
            float u = (float)x / (float)(data->width - 1);
            float v = 1.0f - ((float)y / (float)(data->height - 1)); // Flip Y

            // direction = lower_left + u*hor + v*vert - origin
            t_vec3 dir = lower_left_corner;
            dir = vec_add(dir, vec_scale(horizontal, u));
            dir = vec_add(dir, vec_scale(vertical, v));
            dir = vec_sub(dir, origin);

            t_vec3 color = get_ray_color(origin, dir);

            // Convert to int color
            uint32_t ir = (uint32_t)(255.99f * color.x);
            uint32_t ig = (uint32_t)(255.99f * color.y);
            uint32_t ib = (uint32_t)(255.99f * color.z);
            uint32_t ia = 255;

            uint32_t pixel_color = (ir << 24) | (ig << 16) | (ib << 8) | ia;
            mlx_put_pixel(data->img, x, y, pixel_color);
        }
    }

    return (0);
}
