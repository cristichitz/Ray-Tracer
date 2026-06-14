/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_bonus.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 20:14:55 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 14:25:49 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_BONUS_H
#define RT_BONUS_H

#include "CL/cl.h"
#include "MLX42/MLX42.h"

// Lower resolution = the biggest realtime win (cost is ~quadratic in width).
// Bump these back up for final stills; these values target a smooth animation.
#define BONUS_WIDTH 1280
// Samples taken per rendered frame. Kept low because frames accumulate when
// the camera is still (progressive refinement); this is also the quality used
// while moving.
#define BONUS_SPP 2
#define BONUS_MAX_DEPTH 2
// Stop re-rendering once this many frames have accumulated (converged -> idle).
#define ACCUM_MAX 512
// L spawns an emissive sphere; tune size/strength per scene scale
#define LIGHT_RADIUS 5.0f
#define LIGHT_GAIN 30.0f
// Offline render-to-disk (run with --render): the whole cinematic plays
// automatically and one PPM image per animation frame is written into frames/.
// This decouples the animation from the keyboard, so you can crank samples/depth
// and let it grind out a frame sequence to assemble into a video with ffmpeg.
#define RENDER_SPP 1        // path-tracing samples accumulated per saved frame
#define RENDER_MAX_FRAMES 3000  // safety cap on total frames written
// Simulated seconds per SAVED frame (--render only). Playback at 60fps is
// real-time when this is 1/60 (0.0167); smaller = slow motion. Interactive
// mode keeps using PHYS_DT, which is tuned to feel fast on screen.
#define RENDER_DT 0.008f
//Sphere inclues hittable...
// #include "shape.h"
#include "cl_util_bonus.h"
// #include "sphere.h"
// #include "hittable.h"
#include "init_kernel.h"
// #include "vec3.h"
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

float random_float(float min, float max);
float degrees_to_radians(float degrees);

cl_float3 make_float3(cl_float x, cl_float y, cl_float z);

float   length_squared(cl_float3 a);
cl_float3  random_vec_between(float min, float max);
cl_float3  random_unit_vector(void);
cl_float3  random_on_hemisphere(cl_float3 normal);

bool near_zero(cl_float3 a);
cl_float3 mult(cl_float3 a, cl_float3 b);

cl_float3  make_vec(float a, float b, float c);
cl_float3  add(cl_float3 a, cl_float3 b);
cl_float3  sub(cl_float3 a, cl_float3 b);
cl_float3  scale(cl_float3 a, float b);
cl_float3  divide(cl_float3 a, float b);
float      dot(cl_float3 a, cl_float3 b);
cl_float3  cross(cl_float3 a, cl_float3 b);
cl_float3  norm(cl_float3 a);
void       print_vec(cl_float3 a);

typedef struct s_gpu {
  cl_context        context;
  cl_command_queue  queue;
  cl_program        program;
  cl_kernel         kernel;
  cl_mem            buffer;
  cl_mem            object_buffer;
  cl_mem            accum_buffer;
  cl_mem            node_buffer;
  cl_mem            prim_buffer;
} t_gpu;

/*
** Host-side BVH over every bounded primitive (spheres, quads, cylinders).
** Planes are infinite so they sit at the tail of `prim` and are intersected
** linearly. `prim` indexes into data->objects, which is never reordered, so
** the rubik/physics/stage code (which addresses objects by index) is
** unaffected. Rebuilt from scratch whenever scene_dirty is set.
*/
typedef struct s_bvh {
  t_bvh_node *nodes;       // flat tree, root at 0, capacity 2 * nprim + 1
  int        nnodes;
  int        *prim;        // object indices: BVH prims first, planes after
  int        nprim;        // bounded primitives in the tree
  int        plane_count;  // planes at prim[nprim .. nprim + plane_count)
} t_bvh;

// --- Rigid-body physics ----------------------------------------------------
// Physics is a general property of objects, not of a scene: any object whose
// material is `dynamic` (see material_named) becomes a rigid body, and any
// static plane becomes a collision surface. PHYS_DT is the "how fast it plays"
// knob (simulated time added per rendered frame). PHYS_RESTITUTION / FRICTION
// are only fallbacks; per-contact values come from the two materials touching.
# define BOX_FACES 6            // quads a box rigid body drives
# define PHYS_DT 0.030f         // simulated time per rendered frame
# define PHYS_SUBSTEPS 6        // collision sub-iterations per frame (stability)
# define PHYS_GRAVITY -70.0f    // downward acceleration (world units / s^2)
# define PHYS_RESTITUTION 0.22f // default bounciness of a collision (0..1)
# define PHYS_FRICTION 0.45f    // default tangential friction coefficient
# define PHYS_LIN_DAMP 0.995f   // per-substep linear velocity damping
# define PHYS_ANG_DAMP 0.985f   // per-substep angular velocity damping
# define PHYS_BAUMGARTE 0.30f   // fraction of penetration corrected per contact
# define PHYS_SLOP 0.01f        // penetration allowed before correction kicks in
# define PHYS_SLEEP_VEL 0.8f    // motion below which the scene is settled
# define PHYS_MAX_BODIES 128    // rigid body capacity
# define PHYS_MAX_COLLIDERS 32  // static collision-plane capacity
# define PHYS_LAUNCH_SPEED 80.0f // speed the SPACE shove imparts to a body
# define PHYS_CLICK_IMPULSE 600.0f // impulse a left mouse click pushes with

// Unit quaternion (w + xi + yj + zk); carries a rigid body's orientation.
typedef struct s_quat {
  float  w;
  float  x;
  float  y;
  float  z;
} t_quat;

/*
** One rigid body. It drives a contiguous run of objects in data->objects:
** [obj_first, obj_first + obj_count). A sphere drives a single object; a box
** drives its BOX_FACES quads, whose geometry relative to the body center is
** cached in loc_c/u/v at creation so the box can be re-posed from just
** pos + orient. The inertia is isotropic, so inv_i is a single scalar.
** restitution/friction are copied from the material, so collisions are
** material-driven.
*/
typedef struct s_rbody {
  int        obj_first;   // first object this body drives
  int        obj_count;   // objects driven (1 = sphere, BOX_FACES = box)
  int        shape;       // 0 = box (drives quads), 1 = sphere
  cl_float3  pos;         // center of mass (world)
  cl_float3  vel;         // linear velocity
  t_quat     orient;      // orientation
  cl_float3  omega;       // angular velocity (world)
  float      inv_mass;    // 0 for a static body (immovable)
  float      inv_i;       // 0 for a static body
  float      half;        // box half-extent / sphere radius (see shape)
  float      restitution; // bounciness, copied from the material
  float      friction;    // friction coefficient, copied from the material
  int        sleeping;    // settled and skipped by the integrator
  cl_float3  loc_c[BOX_FACES]; // box: canonical quad corner vs body center
  cl_float3  loc_u[BOX_FACES]; // box: canonical quad u edge
  cl_float3  loc_v[BOX_FACES]; // box: canonical quad v edge
} t_rbody;

// A static collision half-space n . p >= d (n points toward the bodies),
// harvested from every static plane in the scene. restitution/friction feed
// the contact solver just like a body's would.
typedef struct s_collider {
  cl_float3  n;
  float      d;
  float      restitution;
  float      friction;
} t_collider;

typedef struct s_physics {
  t_rbody    bodies[PHYS_MAX_BODIES];
  int        count;
  t_collider colliders[PHYS_MAX_COLLIDERS];
  int        collider_count;
  int        running;     // 1 while the simulation is live
  int        autostart;   // 1 = bodies move on load; else wait for input
  int        settle;      // consecutive near-still frames (-> stop when high)
  float      floor_y;     // fallback ground height when no plane collider exists
} t_physics;

typedef struct s_data {
    void          *mlx;
    mlx_image_t   *img;
    t_physics     phys;

    // Camera as parsed from the .rt file (drives the view basis in initialize)
    cl_float3 cam_center;
    cl_float3 cam_dir;
    float     cam_yaw;
    float     cam_pitch;
    float     cam_fov;

    t_gpu     gpu;
    cl_int    err;

    t_image   frame;

    // Flat list of every primitive in the scene (uploaded as one buffer)
    t_object  *objects;
    uint32_t  obj_count;

    // Acceleration structure over `objects` (see t_bvh)
    t_bvh     bvh;

    // Number of frames accumulated since the last camera move (progressive)
    int       frame_index;

    // Objects changed on the host; re-upload the scene buffer before render
    int       scene_dirty;

    // Offline render-to-disk mode (--render)
    int       render_mode;     // 1 = headless render, auto-play + write frames
    int       render_started;  // the simulation has been auto-triggered
    int       render_frame_no; // index of the next frame to write
} t_data;

// GPU setup / teardown (init_gpu_bonus.c, cleanup_bonus.c)
cl_int     init_gpu(t_data *data);
void       render_frame(t_data *data);
void       game_loop(void *param);
void       clean_gpu(t_data *data);
void       cleanup(void *param);

// Camera key controls (controls_bonus.c)
int        handle_rotation(t_data *data);
int        handle_movement(t_data *data, cl_float3 *step);

// Camera / view (view_bonus.c)
void       initialize(t_data *data);
void       update_view(t_data *data);

// Object + material builders (host side)
typedef struct s_cyl_size
{
	float	radius;
	float	height;
}	t_cyl_size;

t_material material_init(cl_float3 color, int type);
t_object   make_obj_sphere(cl_float3 center, float radius, t_material mat);
t_object   make_obj_plane(cl_float3 point, cl_float3 normal, t_material mat);
t_object   make_obj_quad(cl_float3 q, cl_float3 u, cl_float3 v, t_material mat);
t_object   make_obj_cylinder(cl_float3 center, cl_float3 axis,
                             t_cyl_size size, t_material mat);

// Scene assembly (scene_bonus.c)
int        add_object(t_data *data, t_object obj);
void       bvh_build(t_data *data);
void       obj_bounds(t_object *o, cl_float3 *mn, cl_float3 *mx);
void       bvh_range_bounds(t_data *d, int first, int count, cl_float3 box[2]);
int        make_box(t_data *data, cl_float3 a, cl_float3 b, t_material mat);
void       make_cornell_box(t_data *data);

// Named "physics materials" (material_preset_bonus.c)
t_material material_named(const char *name, cl_float3 color);

// Rigid bodies: registration during build + per-frame posing (body_bonus.c)
void       add_body(t_data *data, int first, int count, t_material mat);
void       body_place(t_data *data, t_rbody *b);

// Quaternion helpers (quat_*_bonus.c)
t_quat     quat_identity(void);
t_quat     quat_axis(int axis, float angle);
t_quat     quat_from_axis(cl_float3 axis, float angle);
t_quat     quat_mul(t_quat a, t_quat b);
cl_float3  quat_apply(t_quat q, cl_float3 v);
t_quat     quat_normalize(t_quat q);
t_quat     quat_integrate(t_quat q, cl_float3 w, float dt);

// Rigid-body physics (src/physics/*.c)
typedef struct s_contact
{
	cl_float3	n;
	cl_float3	ra;
	cl_float3	rb;
	float		jn;
}	t_contact;

void       physics_init(t_data *data);
void       physics_step(t_data *data);
void       collide_world(t_physics *ph, t_rbody *b);
void       collide_plane(t_rbody *b, t_collider c);
void       collide_ground(t_rbody *b, float floor_y);
void       collide_pair(t_rbody *a, t_rbody *b);
void       contact_impulse(t_rbody *a, t_rbody *b, cl_float3 n, cl_float3 cp);
void       collide_ball(t_rbody *a, t_rbody *b);
void       ball_ball(t_rbody *a, t_rbody *b);
void       collide_ball_ground(t_rbody *b, float floor_y);
cl_float3  box_axis(t_rbody *b, int i);
cl_float3  box_vertex(t_rbody *b, int i);
cl_float3  contact_point(t_rbody *a, t_rbody *b);
int        sat_overlap(t_rbody *a, t_rbody *b, cl_float3 *n, float *pen);

// Modular physics input: impulses + ray picking (physics_input_bonus.c)
void       apply_impulse(t_rbody *b, cl_float3 imp, cl_float3 point);
int        pick_body(t_data *data, t_ray ray);
t_ray      ray_from_screen(t_data *data, float sx, float sy);
void       shove_forward(t_data *data);

cl_int     init_objects(t_data *data);

// Offline render-to-disk (render_out_bonus.c)
int        render_mode_on(int argc, char **argv);
void       render_loop(void *param);

// Parsing (parse_*_bonus.c)
int        load_scene(t_data *data, int argc, char **argv);
int        parse_scene(t_data *data, char *file);
int        parse_err(char *msg);
size_t     split_len(char **s);
int        set_vec3(cl_float3 *v, char *s);
int        set_color(cl_float3 *c, char *s);
char       *mat_token(char **p, size_t base);
int        set_cam(t_data *data, char **p);
int        set_ambient(t_data *data, char **p);
int        set_light(t_data *data, char **p);
int        set_sphere(t_data *data, char **p);
int        set_plane(t_data *data, char **p);
int        set_cylinder(t_data *data, char **p);
int        set_box(t_data *data, char **p);
int        set_quad(t_data *data, char **p);
#endif
