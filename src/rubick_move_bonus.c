#include "rt_bonus.h"
#include <stdlib.h>

/*
** Rotate the pair (*a, *b) by (cos, sin). This is the 2D rotation that the
** axis rotations below are built from; using one helper keeps the geometric
** rotation and the integer position snap perfectly consistent.
*/
static void  rot_pair(float *a, float *b, float ct, float st)
{
  float  na;
  float  nb;

  na = ct * *a - st * *b;
  nb = st * *a + ct * *b;
  *a = na;
  *b = nb;
}

/*
** Rotate a vector about a coordinate axis (right-hand rule), in place. The
** cyclic pairing (x->y->z->x) matches the standard rotation matrices.
*/
void  rot_vec_axis(cl_float3 *v, int axis, float ct, float st)
{
  if (axis == 0)
    rot_pair(&v->y, &v->z, ct, st);
  else if (axis == 1)
    rot_pair(&v->z, &v->x, ct, st);
  else
    rot_pair(&v->x, &v->y, ct, st);
}

/*
** Rotate one quad `ang` radians about `axis` through `pivot`, then rebuild the
** cached plane fields the kernel reads (normal, d, w).
*/
void  rotate_quad_axis(t_object *o, int axis, cl_float3 pivot, float ang)
{
  cl_float3  rel;
  cl_float3  n;
  float      ct;
  float      st;

  ct = cosf(ang);
  st = sinf(ang);
  rel = sub(o->center, pivot);
  rot_vec_axis(&rel, axis, ct, st);
  o->center = add(pivot, rel);
  rot_vec_axis(&o->u, axis, ct, st);
  rot_vec_axis(&o->v, axis, ct, st);
  n = cross(o->u, o->v);
  o->normal = norm(n);
  o->d = dot(o->normal, o->center);
  o->w = divide(n, dot(n, n));
}

/*
** Rotate every quad of every cubie in the active slice by `ang`. A cubie is in
** the slice when its grid coordinate on the move axis equals the move layer.
*/
void  rotate_layer(t_rubik *r, t_object *objs, float ang)
{
  int  c;
  int  q;

  c = 0;
  while (c < CUBIES)
  {
    if (r->cubies[c].pos[r->current.axis] == r->current.layer)
    {
      q = 0;
      while (q < FACES)
        rotate_quad_axis(&objs[r->cubies[c].obj + q++],
            r->current.axis, r->center, ang);
    }
    c++;
  }
}

/*
** After an animated turn finishes, advance the integer grid coordinates of the
** moved cubies by the exact 90-degree permutation the turn implies. Reusing
** rot_vec_axis with (cos, sin) = (0, +/-1) keeps it in lockstep with the
** geometry, so positions never drift.
*/
void  snap_layer(t_rubik *r)
{
  cl_float3  p;
  float      st;
  int        c;
  int        k;

  st = 1.0f - 2.0f * (r->current.turns < 0);
  c = 0;
  while (c < CUBIES)
  {
    if (r->cubies[c].pos[r->current.axis] == r->current.layer)
    {
      p = make_float3(r->cubies[c].pos[0], r->cubies[c].pos[1],
          r->cubies[c].pos[2]);
      k = abs(r->current.turns);
      while (k-- > 0)
        rot_vec_axis(&p, r->current.axis, 0.0f, st);
      r->cubies[c].pos[0] = (int)lroundf(p.x);
      r->cubies[c].pos[1] = (int)lroundf(p.y);
      r->cubies[c].pos[2] = (int)lroundf(p.z);
    }
    c++;
  }
}
