#include "rt_bonus.h"

/*
** Place the camera on a body diagonal so it looks at a corner of the cube and
** sees three faces at once (the classic 3/4 view): 45 degrees around vertical
** plus a downward tilt. dir points from the camera toward the cube center, so
** the camera sits one `dist` back along -dir.
*/
static void  rubik_camera(t_data *data)
{
  cl_float3  dir;
  float      dist;

  dir = norm(make_float3(0.0f, 0.0f, 1.0f));
  dist = 18.0f;
  data->cam_center = sub(data->rubik.center, scale(dir, dist));
  data->cam_dir = dir;
  data->cam_fov = 55.0f;
  data->frame.background = make_float3(0.45f, 0.70f, 1.0f);
}

/*
** A sticker is the outward color when the face is on the cube's surface,
** otherwise near-black (the plastic body seen between cubies).
*/
static t_material  sticker(int outer, cl_float3 color)
{
  if (outer)
    return (material_init(color, 0));
  return (material_init(make_float3(0.03f, 0.03f, 0.03f), 0));
}

/*
** Color the 6 quads make_box appended for one cubie. make_box emits faces in
** the order +Z, +X, -Z, -X, +Y, -Y; ax/sg encode which grid component must
** equal +/-1 for that face to be on the cube's surface.
*/
static void  paint_cubie(t_data *data, int base, int *p)
{
  static const int  ax[FACES] = {2, 0, 2, 0, 1, 1};
  static const int  sg[FACES] = {1, 1, -1, -1, 1, -1};
  cl_float3         col[FACES];
  int               f;

  col[0] = make_float3(0.05f, 0.45f, 0.12f);
  col[1] = make_float3(0.75f, 0.05f, 0.05f);
  col[2] = make_float3(0.05f, 0.15f, 0.75f);
  col[3] = make_float3(0.90f, 0.40f, 0.05f);
  col[4] = make_float3(0.90f, 0.90f, 0.90f);
  col[5] = make_float3(0.90f, 0.85f, 0.10f);
  f = 0;
  while (f < FACES)
  {
    data->objects[base + f].material = sticker(p[ax[f]] == sg[f], col[f]);
    f++;
  }
}

/*
** Build one cubie as a box centered on its grid position, paint its stickers,
** and record where its quads live and what grid cell it occupies.
*/
static void  add_cubie(t_data *data, int *pos, float step, int idx)
{
  cl_float3  c;
  float      h;
  int        base;

  h = step * 0.46f;
  c = make_float3(data->rubik.center.x + pos[0] * step,
      data->rubik.center.y + pos[1] * step,
      data->rubik.center.z + pos[2] * step);
  base = (int)data->obj_count;
  make_box(data, make_float3(c.x - h, c.y - h, c.z - h),
      make_float3(c.x + h, c.y + h, c.z + h),
      material_init(make_float3(0.0f, 0.0f, 0.0f), 0));
  paint_cubie(data, base, pos);
  data->rubik.cubies[idx].obj = base;
  data->rubik.cubies[idx].pos[0] = pos[0];
  data->rubik.cubies[idx].pos[1] = pos[1];
  data->rubik.cubies[idx].pos[2] = pos[2];
}

/*
** Assemble the solved 3x3x3 (27 cubies, 162 quads) centered in front of the
** camera. data is memset to zero in main, so the queue/history start empty.
*/
void  build_rubik(t_data *data)
{
  int  pos[3];
  int  idx;

  data->rubik.center = make_float3(-4.0f, 0.0f, 17.0f);
  idx = 0;
  pos[0] = -1;
  while (pos[0] <= 1)
  {
    pos[1] = -1;
    while (pos[1] <= 1)
    {
      pos[2] = -1;
      while (pos[2] <= 1)
      {
        add_cubie(data, pos, 2.2f, idx++);
        pos[2]++;
      }
      pos[1]++;
    }
    pos[0]++;
  }
  // rubik_stage(data);
  rubik_camera(data);
}
