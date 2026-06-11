#include "rt_bonus.h"
#include <stdlib.h>

/*
** The R-key cinematic, in three phases (t_rubik.explode_phase):
**   EXP_OUT : every cubie flies straight out from the solved cube.
**   EXP_SPIN: the cube hangs fully exploded while the camera orbits 720 deg
**             with an ease-in-out (slow, fast, slow) "bullet time" feel.
**   EXP_IN  : the cubies are sucked back in one by one, accelerating like a
**             gravity pull, but landing in a *scrambled* arrangement instead
**             of the solved one. The cube then immediately solves itself.
**
** Each cubie keeps the same 6 quads in data->objects; we rebuild their world
** geometry every frame from one canonical local box (loc_*) plus a per-cubie
** center and orientation (a quaternion), so the scramble lands with correct
** sticker orientations and the normal move engine can solve it cleanly.
*/

/* ----------------------------- quaternions ------------------------------- */

static t_quat  quat_identity(void)
{
  return ((t_quat){1.0f, 0.0f, 0.0f, 0.0f});
}

/* Rotation by `angle` (radians) about a coordinate axis (0=x, 1=y, 2=z). */
static t_quat  quat_axis(int axis, float angle)
{
  t_quat  q;
  float   s;

  s = sinf(angle * 0.5f);
  q.w = cosf(angle * 0.5f);
  q.x = (axis == 0) * s;
  q.y = (axis == 1) * s;
  q.z = (axis == 2) * s;
  return (q);
}

/* Rotation by `angle` (radians) about an arbitrary axis. */
static t_quat  quat_from_axis(cl_float3 axis, float angle)
{
  t_quat  q;
  float   s;

  axis = norm(axis);
  s = sinf(angle * 0.5f);
  q.w = cosf(angle * 0.5f);
  q.x = axis.x * s;
  q.y = axis.y * s;
  q.z = axis.z * s;
  return (q);
}

/* A per-cubie tumble axis, varied but deterministic (and never zero). */
static cl_float3  tumble_axis(int c)
{
  return (make_float3(sinf(c * 1.7f + 0.3f),
        sinf(c * 2.6f + 1.1f), cosf(c * 1.3f + 0.5f)));
}

/* Hamilton product: the rotation "do b, then a". */
static t_quat  quat_mul(t_quat a, t_quat b)
{
  t_quat  q;

  q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
  q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
  q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
  q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
  return (q);
}

/* Rotate a vector by a unit quaternion (q * v * q^-1). */
static cl_float3  quat_apply(t_quat q, cl_float3 v)
{
  t_quat  p;
  t_quat  r;

  p = (t_quat){0.0f, v.x, v.y, v.z};
  r = quat_mul(quat_mul(q, p), (t_quat){q.w, -q.x, -q.y, -q.z});
  return (make_float3(r.x, r.y, r.z));
}

/* slerp(identity, q, t): the same rotation scaled to a fraction t of the way. */
static t_quat  quat_pow(t_quat q, float t)
{
  t_quat  r;
  float   s;
  float   half;

  s = sqrtf(fmaxf(0.0f, 1.0f - q.w * q.w));
  if (s < 1e-6f)
    return (quat_identity());
  half = acosf(fmaxf(-1.0f, fminf(1.0f, q.w))) * t;
  r.w = cosf(half);
  r.x = q.x / s * sinf(half);
  r.y = q.y / s * sinf(half);
  r.z = q.z / s * sinf(half);
  return (r);
}

/* ----------------------------- scramble plan ----------------------------- */

/* Rotate an integer grid coordinate by 90*turns about axis (matches snap). */
static void  rotate_grid(int *p, int axis, int turns)
{
  cl_float3  v;
  float      st;
  int        k;

  st = 1.0f - 2.0f * (turns < 0);
  v = make_float3(p[0], p[1], p[2]);
  k = abs(turns);
  while (k-- > 0)
    rot_vec_axis(&v, axis, 0.0f, st);
  p[0] = (int)lroundf(v.x);
  p[1] = (int)lroundf(v.y);
  p[2] = (int)lroundf(v.z);
}

/*
** Choose SCRAMBLE_LEN random moves and, without animating anything, work out
** where every cubie ends up: its grid cell (target_pos), world center
** (home_target) and net orientation (rot_target). The moves are stored in
** history so solve_rubik can later replay their inverse.
*/
static void  plan_scramble(t_rubik *r)
{
  int     trackpos[CUBIES][3];
  t_quat  trackq[CUBIES];
  t_move  m;
  int     i;
  int     c;

  c = 0;
  while (c < CUBIES)
  {
    trackpos[c][0] = r->cubies[c].pos[0];
    trackpos[c][1] = r->cubies[c].pos[1];
    trackpos[c][2] = r->cubies[c].pos[2];
    trackq[c++] = quat_identity();
  }
  r->h_count = 0;
  i = 0;
  while (i++ < SCRAMBLE_LEN)
  {
    m.axis = rand() % 3;
    m.layer = (rand() % 2) * 2 - 1;
    m.turns = (rand() % 3) - 1;
    if (m.turns == 0)
      m.turns = 2;
    r->history[r->h_count++] = m;
    c = 0;
    while (c < CUBIES)
    {
      if (trackpos[c][m.axis] == m.layer)
      {
        rotate_grid(trackpos[c], m.axis, m.turns);
        trackq[c] = quat_mul(quat_axis(m.axis,
              degrees_to_radians(90.0f * m.turns)), trackq[c]);
      }
      c++;
    }
  }
  c = 0;
  while (c < CUBIES)
  {
    r->target_pos[c][0] = trackpos[c][0];
    r->target_pos[c][1] = trackpos[c][1];
    r->target_pos[c][2] = trackpos[c][2];
    r->rot_target[c] = trackq[c];
    r->home_target[c] = add(r->center, scale(make_float3(trackpos[c][0],
            trackpos[c][1], trackpos[c][2]), RUBIK_STEP));
    c++;
  }
}

/* ------------------------------- launch ---------------------------------- */

/* Capture the solved cube as the canonical box + per-cubie solved homes. */
static void  capture_cube(t_rubik *r, t_object *objs)
{
  cl_float3  c0;
  int        c;
  int        f;

  c0 = add(r->center, scale(make_float3(r->cubies[0].pos[0],
          r->cubies[0].pos[1], r->cubies[0].pos[2]), RUBIK_STEP));
  f = 0;
  while (f < FACES)
  {
    r->loc_c[f] = sub(objs[r->cubies[0].obj + f].center, c0);
    r->loc_u[f] = objs[r->cubies[0].obj + f].u;
    r->loc_v[f] = objs[r->cubies[0].obj + f].v;
    f++;
  }
  c = 0;
  while (c < CUBIES)
  {
    r->home_solved[c] = add(r->center, scale(make_float3(r->cubies[c].pos[0],
            r->cubies[c].pos[1], r->cubies[c].pos[2]), RUBIK_STEP));
    r->expl_dir[c] = norm(make_float3(r->cubies[c].pos[0],
          r->cubies[c].pos[1], r->cubies[c].pos[2]));
    c++;
  }
}

/*
** Kick off the cinematic, but only from a settled, solved cube (so the
** captured geometry is clean and the auto-solve afterwards is exact).
*/
void  start_explode(t_data *data)
{
  t_rubik    *r;
  cl_float3  rel;

  r = &data->rubik;
  if (r->explode_active || r->active || r->q_count != 0 || r->h_count != 0)
    return ;
  capture_cube(r, data->objects);
  plan_scramble(r);
  rel = sub(data->cam_center, r->center);
  r->cam_base_dist = sqrtf(dot(rel, rel));
  r->cam_base_dir = norm(rel);
  r->spin_angle = 0.0f;
  r->explode_phase = EXP_OUT;
  r->explode_frame = 0;
  r->explode_active = 1;
}

/* --------------------------- per-frame update ---------------------------- */

static float  smoothstep(float t)
{
  t = fmaxf(0.0f, fminf(1.0f, t));
  return (t * t * (3.0f - 2.0f * t));
}

static cl_float3  lerp3(cl_float3 a, cl_float3 b, float t)
{
  return (add(scale(a, 1.0f - t), scale(b, t)));
}

/* Rebuild one cubie's 6 quads from the canonical box at a center + orientation. */
static void  place_cubie(t_rubik *r, t_object *objs, int c,
    cl_float3 center, t_quat rot)
{
  t_object   *o;
  cl_float3  n;
  int        f;

  f = 0;
  while (f < FACES)
  {
    o = &objs[r->cubies[c].obj + f];
    o->center = add(center, quat_apply(rot, r->loc_c[f]));
    o->u = quat_apply(rot, r->loc_u[f]);
    o->v = quat_apply(rot, r->loc_v[f]);
    n = cross(o->u, o->v);
    o->normal = norm(n);
    o->d = dot(o->normal, o->center);
    o->w = divide(n, dot(n, n));
    f++;
  }
}

/*
** Place the camera: orbit `spin` radians around Y from its launch direction
** and back it off by `spread` so the whole exploded cube stays framed.
*/
static void  explode_camera(t_data *data, float spread, float spin)
{
  t_rubik    *r;
  cl_float3  rel;

  r = &data->rubik;
  rel = r->cam_base_dir;
  rot_vec_axis(&rel, 1, cosf(spin), sinf(spin));
  rel = scale(rel, r->cam_base_dist + spread * EXPLODE_CAM_PULL);
  data->cam_center = add(r->center, rel);
  data->cam_dir = norm(sub(r->center, data->cam_center));
  data->cam_yaw = atan2f(data->cam_dir.x, data->cam_dir.z);
  data->cam_pitch = asinf(fmaxf(-1.0f, fminf(1.0f, data->cam_dir.y)));
}

/*
** Phase 1: fly the cubies apart (smoothstep so they ease to a stop) while each
** one tumbles. The tumble is a whole number of turns (EXPLODE_OUT_TURNS) so it
** lands back at the identity orientation, matching the spin/implode that follow.
*/
static void  phase_out(t_data *data)
{
  t_rubik  *r;
  float    spread;
  float    tumble;
  int      c;

  r = &data->rubik;
  r->explode_frame++;
  spread = EXPLODE_MAX_DIST
    * smoothstep((float)r->explode_frame / EXPLODE_OUT_FRAMES);
  tumble = EXPLODE_OUT_TURNS * 2.0f * (float)CL_M_PI
    * ((float)r->explode_frame / EXPLODE_OUT_FRAMES);
  c = 0;
  while (c < CUBIES)
  {
    place_cubie(r, data->objects, c,
        add(r->home_solved[c], scale(r->expl_dir[c], spread)),
        quat_from_axis(tumble_axis(c), tumble));
    c++;
  }
  r->spin_angle += EXPLODE_ORBIT_RATE;
  explode_camera(data, spread, r->spin_angle);
  if (r->explode_frame >= EXPLODE_OUT_FRAMES)
  {
    r->explode_phase = EXP_SPIN;
    r->explode_frame = 0;
  }
}

/*
** Phase 2: hold the spread cube and add a 720 deg burst on top of the steady
** orbit, eased slow-fast-slow (bullet time) via the smoothstep velocity curve.
*/
static void  phase_spin(t_data *data)
{
  t_rubik  *r;
  float    burst;

  r = &data->rubik;
  r->explode_frame++;
  burst = EXPLODE_SPIN_TURNS * 2.0f * (float)CL_M_PI
    * (smoothstep((float)r->explode_frame / EXPLODE_SPIN_FRAMES)
      - smoothstep((float)(r->explode_frame - 1) / EXPLODE_SPIN_FRAMES));
  r->spin_angle += EXPLODE_ORBIT_RATE + burst;
  explode_camera(data, EXPLODE_MAX_DIST, r->spin_angle);
  if (r->explode_frame >= EXPLODE_SPIN_FRAMES)
  {
    r->explode_phase = EXP_IN;
    r->explode_frame = 0;
  }
}

/* End of phase 3: commit the scramble and let the cube solve itself. */
static void  finish_explode(t_rubik *r)
{
  int  c;

  c = 0;
  while (c < CUBIES)
  {
    r->cubies[c].pos[0] = r->target_pos[c][0];
    r->cubies[c].pos[1] = r->target_pos[c][1];
    r->cubies[c].pos[2] = r->target_pos[c][2];
    c++;
  }
  r->explode_phase = EXP_IDLE;
  r->explode_active = 0;
  r->orbit_active = 1;
  solve_rubik(r);
}

/*
** Keep the camera orbiting the (now reassembled) cube while it solves itself.
** Called from the move engine each solve frame; step_rubik clears orbit_active
** once the solve finishes.
*/
void  explode_orbit_solve(t_data *data)
{
  data->rubik.spin_angle += EXPLODE_ORBIT_RATE;
  explode_camera(data, 0.0f, data->rubik.spin_angle);
}

/*
** Phase 3: suck the cubies into their scrambled homes one by one. Each cubie
** waits for its staggered launch, then accelerates (k = p*p) into place while
** its orientation slerps from solved to its scrambled target.
*/
static void  phase_in(t_data *data)
{
  t_rubik  *r;
  float    gt;
  float    p;
  int      c;

  r = &data->rubik;
  r->explode_frame++;
  gt = (float)r->explode_frame / EXPLODE_IN_FRAMES;
  c = 0;
  while (c < CUBIES)
  {
    p = (gt - (float)c / CUBIES * EXPLODE_STAGGER)
      / (1.0f - (float)c / CUBIES * EXPLODE_STAGGER);
    p = fmaxf(0.0f, fminf(1.0f, p));
    place_cubie(r, data->objects, c,
        lerp3(add(r->home_solved[c], scale(r->expl_dir[c], EXPLODE_MAX_DIST)),
          r->home_target[c], p * p),
        quat_pow(r->rot_target[c], p * p));
    c++;
  }
  r->spin_angle += EXPLODE_ORBIT_RATE;
  explode_camera(data, EXPLODE_MAX_DIST * (1.0f - smoothstep(gt)),
      r->spin_angle);
  if (r->explode_frame >= EXPLODE_IN_FRAMES)
    finish_explode(r);
}

/* One frame of the cinematic; dispatches on the current phase. */
void  explode_step(t_data *data)
{
  if (data->rubik.explode_phase == EXP_OUT)
    phase_out(data);
  else if (data->rubik.explode_phase == EXP_SPIN)
    phase_spin(data);
  else if (data->rubik.explode_phase == EXP_IN)
    phase_in(data);
  else
    data->rubik.explode_active = 0;
}
