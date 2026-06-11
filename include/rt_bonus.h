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
#define BONUS_SPP 1
#define BONUS_MAX_DEPTH 2
// Stop re-rendering once this many frames have accumulated (converged -> idle).
#define ACCUM_MAX 512
// L spawns an emissive sphere; tune size/strength per scene scale
#define LIGHT_RADIUS 5.0f
#define LIGHT_GAIN 30.0f
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

typedef struct s_data {
    void          *mlx;
    mlx_image_t   *img;
    t_rubik       rubik;

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
} t_data;

// GPU setup / teardown (init_gpu_bonus.c, cleanup_bonus.c)
cl_int     init_gpu(t_data *data);
void       render_frame(t_data *data);
void       game_loop(void *param);
void       clean_gpu(t_data *data);
void       cleanup(void *param);

// Camera / view (view_bonus.c)
void       initialize(t_data *data);
void       update_view(t_data *data);

// Object + material builders (host side)
t_material material_init(cl_float3 color, int type);
t_object   make_obj_sphere(cl_float3 center, float radius, t_material mat);
t_object   make_obj_plane(cl_float3 point, cl_float3 normal, t_material mat);
t_object   make_obj_quad(cl_float3 q, cl_float3 u, cl_float3 v, t_material mat);
t_object   make_obj_cylinder(cl_float3 center, cl_float3 axis, float radius,
                             float height, t_material mat);

// Scene assembly (scene_bonus.c)
int        add_object(t_data *data, t_object obj);
int        make_box(t_data *data, cl_float3 a, cl_float3 b, t_material mat);
void       make_cornell_box(t_data *data);
void       make_rubick_cube(t_data *data);
void       rubik_stage(t_data *data);

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

cl_int     init_objects(t_data *data);

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
