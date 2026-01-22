// ==========================================
// 1. HELPER FUNCTIONS (Refactored)
// ==========================================

// No custom struct or math functions needed!
// We rely entirely on OpenCL built-ins:
// - float3
// - dot(), normalize(), length(), sqrt()
// - Operators (+, -, *) work automatically on vectors

// Returns -1.0 if no hit, or the distance "t" if it hits
float hit_sphere(float3 center, float radius, float3 orig, float3 dir) {
    float3 oc = orig - center;         // Vector subtraction
    float a = dot(dir, dir);           // Built-in dot product
    float b = 2.0f * dot(oc, dir);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return -1.0f;
    } else {
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }
}

// Determine the color of the ray
float3 ray_color(float3 orig, float3 dir) {
    // 1. Check for Sphere Hit
    float3 sphere_center = (float3)(0.0f, 0.0f, -1.0f);
    float t = hit_sphere(sphere_center, 0.5f, orig, dir);

    if (t > 0.0f) {
        // Hit! Calculate normal for shading
        float3 hit_point = orig + (dir * t);
        float3 normal = normalize(hit_point - sphere_center); // Built-in normalize
        
        // Map normal (-1 to 1) to color (0 to 1)
        return 0.5f * (normal + 1.0f);
    }

    // 2. Background (Blue/White Gradient)
    float3 unit_dir = normalize(dir);
    float a = 0.5f * (unit_dir.y + 1.0f);
    
    float3 color1 = (float3)(1.0f, 1.0f, 1.0f); // White
    float3 color2 = (float3)(0.5f, 0.7f, 1.0f); // Blueish
    
    // Linear interpolation: (1-a)*c1 + a*c2
    return (1.0f - a) * color1 + a * color2;
}

__kernel void render_kernel(__global int *img_buffer, int width, int height,
                            float cam_x, float cam_y, float cam_z )
{
    // 1. Get Global ID (1D)
    int x = get_global_id(0);
    int y = get_global_id(1);

    
    // Safety check: Don't write outside the buffer
    if (x >= width || y >= height) return;

    int id = y * width + x;

    // 3. Setup Camera
    float aspect_ratio = (float)width / (float)height;
    float viewport_height = 2.0f;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    float3 origin = (float3)(cam_x, cam_y, cam_z);
    float3 horizontal = (float3)(viewport_width, 0.0f, 0.0f);
    float3 vertical = (float3)(0.0f, viewport_height, 0.0f);
    
    // Lower Left Corner
    float3 lower_left_corner = origin - (horizontal / 2.0f) - (vertical / 2.0f) - (float3)(0.0f, 0.0f, focal_length);

    // 4. Calculate UV
    float u = (float)x / (float)(width - 1);
    float v = 1.0f - ((float)y / (float)(height - 1));

    // 5. Ray Direction
    float3 direction = lower_left_corner + (u * horizontal) + (v * vertical) - origin;

    // 6. Ray Color Logic (Inline for simplicity)
    float3 sphere_center = (float3)(0.0f, 0.0f, -1.0f);
    float radius = 0.5f;
    
    // Hit Sphere?
    float3 oc = origin - sphere_center;
    float a = dot(direction, direction);
    float b = 2.0f * dot(oc, direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    float3 final_color;

    if (discriminant > 0.0f) {
        // Hit: Calculate Normal
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        float3 hit_point = origin + (direction * t);
        float3 normal = normalize(hit_point - sphere_center);
        final_color = 0.5f * (normal + 1.0f);
    } else {
        // Miss: Background Gradient
        float3 unit_dir = normalize(direction);
        float t = 0.5f * (unit_dir.y + 1.0f);
        float3 white = (float3)(1.0f, 1.0f, 1.0f);
        float3 blue = (float3)(0.5f, 0.7f, 1.0f);
        final_color = (1.0f - t) * white + t * blue;
    }

    // 1. Calculate color
    uchar ir = (uchar)(255.99f * final_color.x);
    uchar ig = (uchar)(255.99f * final_color.y);
    uchar ib = (uchar)(255.99f * final_color.z);
    uchar ia = 255;

    // 2. Cast the integer buffer to a vector buffer
    __global uchar4 *ptr = (__global uchar4 *)img_buffer;

    // 3. Write directly (Order is R, G, B, A)
    ptr[id] = (uchar4)(ir, ig, ib, ia);
}