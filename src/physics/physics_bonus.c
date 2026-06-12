#include "rt_bonus.h"
#include <stdlib.h>
#include <string.h>

/*
** The grand finale: once the R-key cinematic has solved the cube, the cubies
** that spell out the "middle finger" (the z=0 face minus its two top corners)
** are frozen in place, and every other cubie drops away as a rigid body that
** collides with the floor, the standing finger and the other falling cubies.
*/

/*
** Finger = a single z=0 slice in a "_|_" shape: the center column (x==0, the
** raised finger) plus the bottom row (y==-1, the base). Five cubies.
*/
int  cubie_is_finger(t_rubik *r, int c)
{
  int  *p;

  p = r->cubies[c].pos;
  if (p[2] != 0)
    return (0);
  return (p[0] == 0 || p[1] == -1);
}

static float  frand(float lo, float hi)
{
  return (lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
}

/* Turn every non-finger cubie into a falling rigid body; freeze the finger. */
void  physics_start(t_data *data)
{
  t_rubik   *r;
  t_rbody   *b;
  float     half;
  int       c;

  r = &data->rubik;
  half = RUBIK_STEP * 0.46f;
  data->phys.floor_y = r->center.y - RUBIK_STEP - half;
  c = 0;
  while (c < CUBIES)
  {
    b = &data->phys.bodies[c];
    memset(b, 0, sizeof(*b));
    b->cubie = c;
    b->half = half;
    b->orient = quat_identity();
    b->pos = add(r->center, scale(make_float3(r->cubies[c].pos[0],
            r->cubies[c].pos[1], r->cubies[c].pos[2]), RUBIK_STEP));
    if (!cubie_is_finger(r, c))
    {
      b->inv_mass = 1.0f;
      b->inv_i = 1.5f / (half * half);
      b->vel = make_float3(frand(-1.5f, 1.5f), frand(-0.5f, 0.5f),
          frand(-1.5f, 1.5f));
      b->omega = make_float3(frand(-5.0f, 5.0f), frand(-5.0f, 5.0f),
          frand(-5.0f, 5.0f));
    }
    else
      b->sleeping = 1;
    c++;
  }
  data->phys.count = CUBIES;
  data->phys.settle = 0;
  data->phys.rocket_frame = 0;
  data->phys.finger_done = 0;
  data->phys.running = 1;
}

/*
** The finger's grand exit, like a helicopter: after a short hold it spins up in
** place about its vertical axis (accelerating), and only once the rotor is
** wound up (PHYS_LIFT_DELAY) does it accelerate upward out of frame, then
** freeze. The cubies move as one rigid group (the spin is applied to both their
** offset from the axis and their orientation).
*/
static void  rocket_finger(t_data *data)
{
  t_physics  *ph;
  t_rubik    *r;
  t_quat     rot;
  float      lift;
  float      rise;
  float      spin;
  cl_float3  off;
  int        c;

  ph = &data->phys;
  r = &data->rubik;
  if (ph->finger_done || ++ph->rocket_frame <= PHYS_ROCKET_DELAY)
    return ;
  spin = (float)(ph->rocket_frame - PHYS_ROCKET_DELAY);
  spin = 0.5f * PHYS_SPIN_ACCEL * spin * spin;
  lift = fmaxf(0.0f, (float)(ph->rocket_frame - PHYS_LIFT_DELAY));
  rise = 0.5f * PHYS_RISE_ACCEL * lift * lift;
  if (rise > PHYS_ROCKET_MAX_H)
  {
    rise = PHYS_ROCKET_MAX_H;
    ph->finger_done = 1;
  }
  rot = quat_axis(1, spin);
  c = -1;
  while (++c < ph->count)
  {
    if (!cubie_is_finger(r, c))
      continue ;
    off = quat_apply(rot, make_float3(r->cubies[c].pos[0] * RUBIK_STEP,
          0.0f, r->cubies[c].pos[2] * RUBIK_STEP));
    ph->bodies[c].pos = make_float3(r->center.x + off.x, r->center.y
        + r->cubies[c].pos[1] * RUBIK_STEP + rise + off.y, r->center.z + off.z);
    ph->bodies[c].orient = rot;
  }
}

/* Semi-implicit Euler step for one dynamic body. */
static void  integrate(t_rbody *b, float dt)
{
  if (b->inv_mass == 0.0f || b->sleeping)
    return ;
  b->vel.y += PHYS_GRAVITY * dt;
  b->vel = scale(b->vel, PHYS_LIN_DAMP);
  b->omega = scale(b->omega, PHYS_ANG_DAMP);
  b->pos = add(b->pos, scale(b->vel, dt));
  b->orient = quat_integrate(b->orient, b->omega, dt);
}

/* One collision sub-iteration: integrate, then resolve ground + every pair. */
static void  substep(t_physics *ph, float dt)
{
  int  i;
  int  j;

  i = 0;
  while (i < ph->count)
  {
    integrate(&ph->bodies[i], dt);
    if (ph->bodies[i].inv_mass > 0.0f)
      collide_ground(&ph->bodies[i], ph->floor_y);
    i++;
  }
  i = 0;
  while (i < ph->count)
  {
    j = i + 1;
    while (j < ph->count)
      collide_pair(&ph->bodies[i], &ph->bodies[j++]);
    i++;
  }
}

/* Largest linear+angular speed among the dynamic bodies (for sleep detection). */
static float  peak_motion(t_physics *ph)
{
  float  m;
  float  s;
  int    i;

  m = 0.0f;
  i = 0;
  while (i < ph->count)
  {
    if (ph->bodies[i].inv_mass > 0.0f)
    {
      s = dot(ph->bodies[i].vel, ph->bodies[i].vel)
        + dot(ph->bodies[i].omega, ph->bodies[i].omega);
      if (s > m)
        m = s;
    }
    i++;
  }
  return (sqrtf(m));
}

/* Advance the simulation one rendered frame and push the result to geometry. */
void  physics_step(t_data *data)
{
  t_physics  *ph;
  int        s;
  int        c;

  ph = &data->phys;
  if (!ph->running)
    return ;
  rocket_finger(data);
  s = 0;
  while (s++ < PHYS_SUBSTEPS)
    substep(ph, PHYS_DT / PHYS_SUBSTEPS);
  c = 0;
  while (c < ph->count)
  {
    rubik_place_cubie(&data->rubik, data->objects, ph->bodies[c].cubie,
        ph->bodies[c].pos, ph->bodies[c].orient);
    c++;
  }
  if (peak_motion(ph) < PHYS_SLEEP_VEL)
    ph->settle++;
  else
    ph->settle = 0;
  if (ph->finger_done && ph->settle > 30)
    ph->running = 0;
}
