#include "rt_bonus.h"

/*
** Small unit-quaternion toolkit shared by the explode cinematic and the
** rigid-body physics. A quaternion carries an orientation; rotating a vector
** by it is q * (0,v) * q^-1.
*/

t_quat  quat_identity(void)
{
  return ((t_quat){1.0f, 0.0f, 0.0f, 0.0f});
}

/* Rotation by `angle` (radians) about a coordinate axis (0=x, 1=y, 2=z). */
t_quat  quat_axis(int axis, float angle)
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
t_quat  quat_from_axis(cl_float3 axis, float angle)
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

/* Hamilton product: the rotation "do b, then a". */
t_quat  quat_mul(t_quat a, t_quat b)
{
  t_quat  q;

  q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
  q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
  q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
  q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
  return (q);
}

/* Rotate a vector by a unit quaternion (q * v * q^-1). */
cl_float3  quat_apply(t_quat q, cl_float3 v)
{
  t_quat  p;
  t_quat  r;

  p = (t_quat){0.0f, v.x, v.y, v.z};
  r = quat_mul(quat_mul(q, p), (t_quat){q.w, -q.x, -q.y, -q.z});
  return (make_float3(r.x, r.y, r.z));
}

/* slerp(identity, q, t): the same rotation scaled to a fraction t of the way. */
t_quat  quat_pow(t_quat q, float t)
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

t_quat  quat_normalize(t_quat q)
{
  float  len;

  len = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z) + 1e-12f;
  return ((t_quat){q.w / len, q.x / len, q.y / len, q.z / len});
}

/* Advance an orientation by angular velocity w over dt (then renormalize). */
t_quat  quat_integrate(t_quat q, cl_float3 w, float dt)
{
  t_quat  d;

  d = quat_mul((t_quat){0.0f, w.x, w.y, w.z}, q);
  q.w += 0.5f * dt * d.w;
  q.x += 0.5f * dt * d.x;
  q.y += 0.5f * dt * d.y;
  q.z += 0.5f * dt * d.z;
  return (quat_normalize(q));
}

/* Rebuild one cubie's 6 quads from the canonical box at a center + orientation. */
void  rubik_place_cubie(t_rubik *r, t_object *objs, int c,
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
