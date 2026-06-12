#include "rt_bonus.h"
#include <string.h>

/*
** Collision detection + impulse resolution for the falling cubies. Each cubie
** is an oriented box (OBB). Boxes are separated/penetrated with the Separating
** Axis Theorem; contacts are resolved with sequential impulses (normal bounce
** + Coulomb friction) plus Baumgarte positional correction so stacks don't
** sink. The cube's inertia is isotropic, so inv_i is a single scalar.
*/

/* The i-th body axis (its local x/y/z rotated into the world). */
static cl_float3  box_axis(t_rbody *b, int i)
{
  if (i == 0)
    return (quat_apply(b->orient, make_float3(1.0f, 0.0f, 0.0f)));
  if (i == 1)
    return (quat_apply(b->orient, make_float3(0.0f, 1.0f, 0.0f)));
  return (quat_apply(b->orient, make_float3(0.0f, 0.0f, 1.0f)));
}

/* World position of corner i (bits select the +/- half on each local axis). */
static cl_float3  box_vertex(t_rbody *b, int i)
{
  cl_float3  local;

  local = make_float3((i & 1) ? b->half : -b->half,
      (i & 2) ? b->half : -b->half, (i & 4) ? b->half : -b->half);
  return (add(b->pos, quat_apply(b->orient, local)));
}

static int  point_in_box(t_rbody *b, cl_float3 p)
{
  cl_float3  local;
  float      e;

  local = quat_apply((t_quat){b->orient.w, -b->orient.x, -b->orient.y,
      -b->orient.z}, sub(p, b->pos));
  e = b->half + 1e-4f;
  return (fabsf(local.x) <= e && fabsf(local.y) <= e && fabsf(local.z) <= e);
}

/* Add velocity + angular velocity from an impulse applied at offset r. */
static void  body_apply(t_rbody *b, cl_float3 imp, cl_float3 r)
{
  b->vel = add(b->vel, scale(imp, b->inv_mass));
  b->omega = add(b->omega, scale(cross(r, imp), b->inv_i));
}

/*
** Resolve one contact point cp with normal n (pointing from a to b): a normal
** impulse (restitution) followed by a clamped friction impulse.
*/
static void  contact_impulse(t_rbody *a, t_rbody *b, cl_float3 n, cl_float3 cp)
{
  cl_float3  ra;
  cl_float3  rb;
  cl_float3  relv;
  cl_float3  t;
  float      v[4];

  ra = sub(cp, a->pos);
  rb = sub(cp, b->pos);
  relv = sub(add(b->vel, cross(b->omega, rb)), add(a->vel, cross(a->omega, ra)));
  v[0] = dot(relv, n);
  if (v[0] >= 0.0f)
    return ;
  v[1] = a->inv_mass + b->inv_mass + a->inv_i * dot(cross(ra, n), cross(ra, n))
    + b->inv_i * dot(cross(rb, n), cross(rb, n));
  if (v[1] < 1e-8f)
    return ;
  v[2] = -(1.0f + PHYS_RESTITUTION) * v[0] / v[1];
  body_apply(a, scale(n, -v[2]), ra);
  body_apply(b, scale(n, v[2]), rb);
  relv = sub(add(b->vel, cross(b->omega, rb)), add(a->vel, cross(a->omega, ra)));
  t = sub(relv, scale(n, dot(relv, n)));
  if (dot(t, t) < 1e-10f)
    return ;
  t = norm(t);
  v[3] = -dot(relv, t) / (a->inv_mass + b->inv_mass
      + a->inv_i * dot(cross(ra, t), cross(ra, t))
      + b->inv_i * dot(cross(rb, t), cross(rb, t)));
  v[3] = fmaxf(-PHYS_FRICTION * v[2], fminf(PHYS_FRICTION * v[2], v[3]));
  body_apply(a, scale(t, -v[3]), ra);
  body_apply(b, scale(t, v[3]), rb);
}

/* The ground is an immovable horizontal plane; each sunk corner is a contact. */
void  collide_ground(t_rbody *b, float floor_y)
{
  t_rbody    ground;
  cl_float3  corner;
  float      maxpen;
  int        i;

  memset(&ground, 0, sizeof(ground));
  maxpen = 0.0f;
  i = 0;
  while (i < 8)
  {
    corner = box_vertex(b, i++);
    if (corner.y < floor_y)
    {
      ground.pos = make_float3(corner.x, floor_y, corner.z);
      contact_impulse(&ground, b, make_float3(0.0f, 1.0f, 0.0f), corner);
      if (floor_y - corner.y > maxpen)
        maxpen = floor_y - corner.y;
    }
  }
  if (maxpen > PHYS_SLOP)
    b->pos.y += (maxpen - PHYS_SLOP) * PHYS_BAUMGARTE;
}

/*
** Test candidate axis l. Returns 0 if it separates the boxes; otherwise keeps
** the smallest overlap so far in mp[0] and its (normalized) axis in mp[1..3].
*/
static int  sat_axis(t_rbody *a, t_rbody *b, cl_float3 l, float *mp)
{
  cl_float3  d;
  float      proj[3];

  if (dot(l, l) < 1e-6f)
    return (1);
  l = norm(l);
  d = sub(b->pos, a->pos);
  proj[0] = a->half * (fabsf(dot(box_axis(a, 0), l)) + fabsf(dot(box_axis(a, 1),
          l)) + fabsf(dot(box_axis(a, 2), l)));
  proj[1] = b->half * (fabsf(dot(box_axis(b, 0), l)) + fabsf(dot(box_axis(b, 1),
          l)) + fabsf(dot(box_axis(b, 2), l)));
  proj[2] = proj[0] + proj[1] - fabsf(dot(d, l));
  if (proj[2] < 0.0f)
    return (0);
  if (proj[2] < mp[0])
  {
    mp[0] = proj[2];
    mp[1] = l.x;
    mp[2] = l.y;
    mp[3] = l.z;
  }
  return (1);
}

/* Full 15-axis SAT. Fills *n (a->b) and *pen on overlap; returns 0 if apart. */
static int  sat_overlap(t_rbody *a, t_rbody *b, cl_float3 *n, float *pen)
{
  float  mp[4];
  int    i;
  int    j;

  mp[0] = 1e30f;
  mp[1] = 0.0f;
  mp[2] = 1.0f;
  mp[3] = 0.0f;
  i = -1;
  while (++i < 3)
    if (!sat_axis(a, b, box_axis(a, i), mp) || !sat_axis(a, b, box_axis(b, i),
        mp))
      return (0);
  i = -1;
  while (++i < 3)
  {
    j = -1;
    while (++j < 3)
      if (!sat_axis(a, b, cross(box_axis(a, i), box_axis(b, j)), mp))
        return (0);
  }
  *n = make_float3(mp[1], mp[2], mp[3]);
  if (dot(sub(b->pos, a->pos), *n) < 0.0f)
    *n = scale(*n, -1.0f);
  *pen = mp[0];
  return (1);
}

/* Approximate the contact as the centroid of mutually-contained corners. */
static cl_float3  contact_point(t_rbody *a, t_rbody *b)
{
  cl_float3  acc;
  cl_float3  v;
  int        cnt;
  int        i;

  acc = make_float3(0.0f, 0.0f, 0.0f);
  cnt = 0;
  i = -1;
  while (++i < 8)
  {
    v = box_vertex(b, i);
    if (point_in_box(a, v))
    {
      acc = add(acc, v);
      cnt++;
    }
    v = box_vertex(a, i);
    if (point_in_box(b, v))
    {
      acc = add(acc, v);
      cnt++;
    }
  }
  if (cnt == 0)
    return (scale(add(a->pos, b->pos), 0.5f));
  return (scale(acc, 1.0f / cnt));
}

void  collide_pair(t_rbody *a, t_rbody *b)
{
  cl_float3  n;
  float      pen;
  float      corr;
  float      ti;

  n = make_float3(0.0f, 1.0f, 0.0f);
  pen = 0.0f;
  if (a->inv_mass == 0.0f && b->inv_mass == 0.0f)
    return ;
  if (!sat_overlap(a, b, &n, &pen))
    return ;
  contact_impulse(a, b, n, contact_point(a, b));
  ti = a->inv_mass + b->inv_mass;
  corr = fmaxf(0.0f, pen - PHYS_SLOP) * PHYS_BAUMGARTE;
  if (ti > 0.0f)
  {
    a->pos = sub(a->pos, scale(n, corr * a->inv_mass / ti));
    b->pos = add(b->pos, scale(n, corr * b->inv_mass / ti));
  }
}
