/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_bonus.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 20:14:55 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/14 14:24:22 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_BONUS_H
#define RT_BONUS_H

#include "CL/cl.h"
#include "MLX42/MLX42.h"

// Lower resolution = the biggest realtime win (cost is ~quadratic in width).
// Bump these back up for final stills; these values target a smooth animation.
#define BONUS_WIDTH 1920
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
} t_gpu;

// --- Rubik's cube demo (host side) -----------------------------------------
// A real 3x3x3: 27 cubies, each built as a 6-quad box. A "move" turns one
// outer slice 90/180 degrees. "Solve" replays the inverse of every move that
// has been applied since the cube was last solved (reverse-scramble), so no
// solving algorithm is needed and the cube always returns to the start state.
# define CUBIES 27
# define FACES 6
# define MOVE_FRAMES 6      // animation frames spent on one 90-degree turn
# define RUBIK_STEP 2.2f    // world-space spacing between adjacent cubie centers
// --- R-key cinematic: explode out -> 720deg orbit -> implode into a scramble.
# define EXPLODE_OUT_FRAMES 25  // frames spent flying the cube apart
# define EXPLODE_SPIN_FRAMES 90 // frames of the camera orbit at full spread
# define EXPLODE_IN_FRAMES 50   // frames spent sucking the cubies into scramble
# define EXPLODE_MAX_DIST 16.0f // peak outward spread of a cubie (world units)
# define EXPLODE_CAM_PULL 1.4f  // how far the camera backs off per unit of spread
# define EXPLODE_SPIN_TURNS 2.0f // full camera revolutions during the spin (720)
# define EXPLODE_STAGGER 0.55f  // fraction of the implode used to launch cubies
# define EXPLODE_ORBIT_RATE 0.025f // steady camera orbit per frame (out/in/solve)
# define EXPLODE_OUT_TURNS 1.0f // tumbles each cubie makes while flying out

// --- Rigid-body physics: after the solve, non-finger cubies fall and collide.
// PHYS_DT is the real "how fast it plays" knob: it is the simulated time added
// per RENDERED frame, so raising it speeds everything up even when the GPU
// render is the bottleneck. PHYS_GRAVITY only changes the fall acceleration.
# define PHYS_DT 0.030f         // simulated time per rendered frame
# define PHYS_SUBSTEPS 6        // collision sub-iterations per frame (stability)
# define PHYS_GRAVITY -70.0f    // downward acceleration (world units / s^2)
# define PHYS_RESTITUTION 0.22f // bounciness of a collision (0..1)
# define PHYS_FRICTION 0.45f    // tangential friction coefficient at contacts
# define PHYS_LIN_DAMP 0.995f   // per-substep linear velocity damping
# define PHYS_ANG_DAMP 0.985f   // per-substep angular velocity damping
# define PHYS_BAUMGARTE 0.30f   // fraction of penetration corrected per contact
# define PHYS_SLOP 0.01f        // penetration allowed before correction kicks in
# define PHYS_SLEEP_VEL 0.8f    // motion below which the pile is considered settled

// --- Finger "rocket": the finger spins up like a helicopter rotor, then lifts.
# define PHYS_ROCKET_DELAY 30   // frames the finger holds still before it spins
# define PHYS_LIFT_DELAY 100    // frames before it stops hovering and starts rising
# define PHYS_SPIN_ACCEL 0.012f // spin acceleration about its axis (rad / frame^2)
# define PHYS_RISE_ACCEL 0.020f // upward acceleration once lifting (units / frame^2)
# define PHYS_ROCKET_MAX_H 70.0f // height at which the finger is gone; freeze it

// --- Stage: a ground quad + two emissive lights in the back corners (one warm,
// one bluish), 45 deg above/behind the cube, that scale up with the explosion
// so the lighting stays proportional to the spreading scene.
// NOTE: keep the lights farther from the cube than the camera's orbit radius
// (cam_base_dist + EXPLODE_MAX_DIST*EXPLODE_CAM_PULL ~ 40) or the orbiting
// camera passes *inside* an emissive sphere and the whole frame turns white.
# define STAGE_LIGHT_HEIGHT 22.0f // how high the lights sit above the cube
# define STAGE_LIGHT_BACK 18.0f   // how far behind the cube (away from camera)
# define STAGE_LIGHT_SPREAD 20.0f // left/right separation of the two lights
# define STAGE_LIGHT_RADIUS 4.0f  // emissive sphere radius at scale 1
# define STAGE_FLOOR_HALF 24.0f   // half-size of the ground quad at scale 1
# define SCENE_SCALE_MAX 1.5f     // peak expansion of the stage during the blast
# define SCRAMBLE_LEN 20     // random moves added by one scramble
# define MAX_MOVES 1024      // ring-buffer / history capacity

// Phases of the R-key cinematic (t_rubik.explode_phase).
# define EXP_IDLE 0
# define EXP_OUT  1
# define EXP_SPIN 2
# define EXP_IN   3

typedef struct s_move {
  int  axis;   // rotation axis: 0 = x, 1 = y, 2 = z
  int  layer;  // which outer slice: -1 or +1
  int  turns;  // +1 = 90, -1 = -90, +2 = 180 (about axis, right-hand rule)
} t_move;

// Unit quaternion (w + xi + yj + zk); used to carry each cubie's net
// orientation through the explode->scramble cinematic.
typedef struct s_quat {
  float  w;
  float  x;
  float  y;
  float  z;
} t_quat;

typedef struct s_cubie {
  int  pos[3]; // current grid coordinate, each component in {-1, 0, 1}
  int  obj;    // index of this cubie's first quad in data->objects
} t_cubie;

typedef struct s_rubik {
  t_cubie    cubies[CUBIES];
  cl_float3  center;            // world-space pivot of the whole cube
  t_move     queue[MAX_MOVES];  // moves waiting to play (ring buffer)
  int        q_head;
  int        q_count;
  t_move     history[MAX_MOVES];// moves applied since solved (for solve)
  int        h_count;
  int        active;            // 1 while a turn is animating
  int        frames_left;       // frames remaining in the active turn
  t_move     current;           // turn being animated
  float      step;              // per-frame angle of the active turn (radians)
  int        explode_active;    // 1 while the R-key cinematic is running

  // --- R-key cinematic state (explode out -> orbit -> implode to scramble) ---
  int        explode_phase;     // EXP_IDLE / EXP_OUT / EXP_SPIN / EXP_IN
  int        explode_frame;     // frame counter within the current phase
  cl_float3  loc_c[FACES];      // canonical quad corner, relative to cubie center
  cl_float3  loc_u[FACES];      // canonical quad u edge
  cl_float3  loc_v[FACES];      // canonical quad v edge
  cl_float3  home_solved[CUBIES];   // cubie center in the solved cube
  cl_float3  home_target[CUBIES];   // cubie center after the scramble
  cl_float3  expl_dir[CUBIES];      // outward explosion direction per cubie
  t_quat     rot_target[CUBIES];    // cubie orientation after the scramble
  int        target_pos[CUBIES][3]; // grid coordinate after the scramble
  cl_float3  cam_base_dir;      // camera->center direction captured at launch
  float      cam_base_dist;     // camera distance captured at launch
  float      spin_angle;        // accumulated orbit angle (radians)
  int        orbit_active;      // 1 while the camera orbits during the auto-solve
} t_rubik;

// One falling cubie as a rigid body. The cube's inertia is isotropic, so its
// inverse inertia is a single scalar (inv_i) rather than a tensor.
typedef struct s_rbody {
  int        cubie;     // index into rubik.cubies (which 6 quads to drive)
  cl_float3  pos;       // center of mass (world)
  cl_float3  vel;       // linear velocity
  t_quat     orient;    // orientation
  cl_float3  omega;     // angular velocity (world)
  float      inv_mass;  // 0 for the frozen "finger" cubies (immovable)
  float      inv_i;     // 0 for the frozen "finger" cubies
  float      half;      // cube half-extent (for corner / OBB tests)
  int        sleeping;  // settled and skipped by the integrator
} t_rbody;

typedef struct s_physics {
  t_rbody  bodies[CUBIES];
  int      count;
  int      running;     // 1 while the simulation is live
  int      settle;      // consecutive near-still frames (-> stop when high)
  int      rocket_frame;// frames since the finger-rocket sequence began
  int      finger_done; // 1 once the finger has lifted out of view
  float    floor_y;     // ground plane height
} t_physics;

typedef struct s_data {
    void          *mlx;
    mlx_image_t   *img;
    t_rubik       rubik;
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

    // Number of frames accumulated since the last camera move (progressive)
    int       frame_index;

    // Offline render-to-disk mode (--render)
    int       render_mode;     // 1 = headless render, auto-play + write frames
    int       render_started;  // the cinematic has been auto-triggered
    int       render_frame_no; // index of the next frame to write

    // Stage props (floor + 2 lights) that scale with the explosion
    int       has_stage;
    int       stage_floor;     // object index of the ground quad
    int       stage_light[2];  // object indices of the two lights
    cl_float3 light_base[2];   // light offset from cube center at scale 1
    float     stage_floor_y;   // ground height
    float     scene_scale;     // current stage expansion factor
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
int        make_box(t_data *data, cl_float3 a, cl_float3 b, t_material mat);
void       make_cornell_box(t_data *data);
void       make_rubick_cube(t_data *data);
void       add_stage(t_data *data);
void       update_stage(t_data *data);

// Rubik's cube: build + move engine (rubick_*_bonus.c)
void       rot_vec_axis(cl_float3 *v, int axis, float ct, float st);
void       explode_step(t_data *data);
void       start_explode(t_data *data);
void       explode_orbit_solve(t_data *data);
void       build_rubik(t_data *data);
void       rotate_quad_axis(t_object *o, int axis, cl_float3 pivot, float ang);
void       rotate_layer(t_rubik *r, t_object *objs, float ang);
void       snap_layer(t_rubik *r);
void       step_rubik(t_data *data);
void       scramble_rubik(t_rubik *r);
void       solve_rubik(t_rubik *r);

// Explode cinematic internals shared across the rubick_*_bonus.c files
void       plan_scramble(t_rubik *r);
float      smoothstep(float t);
void       explode_camera(t_data *data, float spread, float spin);
void       phase_in(t_data *data);

// Shared quaternion + cubie-geometry helpers (quat_*_bonus.c)
typedef struct s_place
{
	int			cubie;
	cl_float3	center;
	t_quat		rot;
}	t_place;

t_quat     quat_identity(void);
t_quat     quat_axis(int axis, float angle);
t_quat     quat_from_axis(cl_float3 axis, float angle);
t_quat     quat_mul(t_quat a, t_quat b);
cl_float3  quat_apply(t_quat q, cl_float3 v);
t_quat     quat_pow(t_quat q, float t);
t_quat     quat_normalize(t_quat q);
t_quat     quat_integrate(t_quat q, cl_float3 w, float dt);
void       rubik_place_cubie(t_rubik *r, t_object *objs, t_place pl);

// Rigid-body physics: the post-solve "middle finger" collapse
// (src/physics/*.c)
typedef struct s_contact
{
	cl_float3	n;
	cl_float3	ra;
	cl_float3	rb;
	float		jn;
}	t_contact;

int        cubie_is_finger(t_rubik *r, int c);
void       physics_start(t_data *data);
void       physics_step(t_data *data);
void       rocket_finger(t_data *data);
void       collide_ground(t_rbody *b, float floor_y);
void       collide_pair(t_rbody *a, t_rbody *b);
cl_float3  box_axis(t_rbody *b, int i);
cl_float3  box_vertex(t_rbody *b, int i);
cl_float3  contact_point(t_rbody *a, t_rbody *b);
int        sat_overlap(t_rbody *a, t_rbody *b, cl_float3 *n, float *pen);

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
int        set_cam(t_data *data, char **p);
int        set_ambient(t_data *data, char **p);
int        set_light(t_data *data, char **p);
int        set_sphere(t_data *data, char **p);
int        set_plane(t_data *data, char **p);
int        set_cylinder(t_data *data, char **p);
#endif
