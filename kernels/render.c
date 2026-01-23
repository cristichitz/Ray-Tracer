#include "structs.h"

// float hit_sphere(float3 center, float radius, float3 orig, float3 dir) {
//     float3 oc = center - orig;
//     float a = dot(dir, dir);
//     float h = dot(dir, oc);
//     float c = dot(oc, oc) - radius * radius;
//     float discriminant = h*h - a * c;

//     if (discriminant < 0) {
//         return -1.0f;
//     } else {
//         // -b - sqrt(b^2 - 4ac) / (2a)
//         return (h - sqrt(discriminant)) / a;
//     }
// }

// Helper: Checks collision with ONE sphere
bool hit_sphere(t_sphere s, float3 r_orig, float3 r_dir, float t_min, float t_max, t_hit_record *rec) 
{
    float3 oc = s.center - r_orig; // Careful with direction (Center - Orig)
    float a = dot(r_dir, r_dir);
    float h = dot(r_dir, oc);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = h*h - a*c;

    if (discriminant < 0) return false;

    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range
    float root = (h - sqrtd) / a;
    if (root <= t_min || root >= t_max) {
        root = (h + sqrtd) / a;
        if (root <= t_min || root >= t_max)
            return false;
    }

    rec->t = root;
    rec->p = r_orig + (r_dir * rec->t);
    rec->normal = (rec->p - s.center) / s.radius;
    
    // Face determination (Check if we hit outside or inside)
    if (dot(r_dir, rec->normal) > 0.0f) {
        rec->normal = -rec->normal; // Inside
        rec->front_face = 0;
    } else {
        rec->front_face = 1;        // Outside
    }
    
    return true;
}

float3  background_gradient(float3 dir)
{
    float3  unit_dir;
    float   a;
    float3  white;
    float3  blue;
    
    unit_dir = normalize(dir);
    a = 0.5f * (unit_dir.y + 1.0f);
    white = (float3)(1.0f, 1.0f, 1.0f);
    blue = (float3)(0.5f, 0.7f, 1.0f);
    return (1.0f - a) * white + a * blue;
}

// // Loops over ALL spheres to find the closest hit
bool hit_world(__global t_sphere* spheres, int count, float3 r_orig, float3 r_dir, t_hit_record *rec)
{
    bool hit_anything = false;
    float closest_so_far = 1e30f; // Represents "Infinity"
    
    t_hit_record temp_rec; // A temporary clipboard for checks

    for (int i = 0; i < count; i++) 
    {
        // We pass 'closest_so_far' as t_max! 
        // This ensures we ignore anything behind the current closest object.
        t_sphere current_sphere = spheres[i];
        
        if (hit_sphere(current_sphere, r_orig, r_dir, 0.001f, closest_so_far, &temp_rec)) 
        {
            hit_anything = true;
            closest_so_far = temp_rec.t; // Shrink the window
            *rec = temp_rec;             // Copy temp details to the final clipboard
        }
    }
    return hit_anything;
}

float3 ray_color(float3 orig, float3 dir, __global t_sphere* spheres, int count) {
    // float3 sphere_center = (float3)(0.0f, 0.0f, -1.0f);
    // float t = hit_sphere(sphere_center, 0.5f, orig, dir);

    // if (t > 0.0f) {
    //     // Hit! Calculate normal for shading
    //     float3 hit_point = orig + (dir * t);
    //     float3 normal = normalize(hit_point - sphere_center);
    //     // Map normal (-1 to 1) to color (0 to 1)
    //     return 0.5f * (normal + 1.0f);
    // }

    t_hit_record    rec;

    if (hit_world(spheres, count, orig, dir, &rec))
    {
        return 0.5f * (rec.normal + 1.0f);
    }
    // Inside render_kernel, replace the hit_world call with this:

    // 2. Background (Blue/White Gradient)
    return (background_gradient(dir));
}

__kernel void render_kernel(__global int *img_buffer, int width, int height, int sphere_count,
                            __global t_sphere* spheres, t_camera cam)
{
    // 1. Get Global ID (1D)
    int x = get_global_id(0);
    int y = get_global_id(1);

    
    // Safety check: Don't write outside the buffer
    if (x >= width || y >= height) return;

    int id = y * width + x;

    // 4. Calculate UV
    float3 pixel_delta_u = cam.pixel_delta_u;
    float3 pixel_delta_v = cam.pixel_delta_v;

    // 5. Ray Direction
    float3 pixel00_loc = cam.pixel00_loc;
    float3 origin = cam.origin;

    float3 pixel_center = pixel00_loc + ((float)x * pixel_delta_u) + ((float)y * pixel_delta_v);
    float3 direction = pixel_center - origin;
    
    float3 final_color = ray_color(origin, direction, spheres, sphere_count);

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