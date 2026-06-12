#include "rt_bonus.h"

/*
** Append one primitive to the scene's flat array, growing it by one slot.
** Returns 1 on success, 0 on allocation failure.
*/
int  add_object(t_data *data, t_object obj)
{
  t_object *grown;

  grown = (t_object *)realloc(data->objects,
      sizeof(t_object) * (data->obj_count + 1));
  if (!grown)
    return (parse_err("Failed to grow object array."));
  data->objects = grown;
  data->objects[data->obj_count] = obj;
  data->obj_count++;
  return (1);
}

/*
** Axis-aligned box as 6 quads. Ported from make_box() in src/class/plane.c.
*/
int  make_box(t_data *data, cl_float3 a, cl_float3 b, t_material mat)
{
  cl_float3 mn;
  cl_float3 mx;
  cl_float3 dx;
  cl_float3 dy;
  cl_float3 dz;

  mn = make_float3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
  mx = make_float3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
  dx = make_float3(mx.x - mn.x, 0.0f, 0.0f);
  dy = make_float3(0.0f, mx.y - mn.y, 0.0f);
  dz = make_float3(0.0f, 0.0f, mx.z - mn.z);
  if (!add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mx.z), dx, dy, mat))
    || !add_object(data, make_obj_quad(make_float3(mx.x, mn.y, mx.z), scale(dz, -1), dy, mat))
    || !add_object(data, make_obj_quad(make_float3(mx.x, mn.y, mn.z), scale(dx, -1), dy, mat))
    || !add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mn.z), dz, dy, mat))
    || !add_object(data, make_obj_quad(make_float3(mn.x, mx.y, mx.z), dx, scale(dz, -1), mat))
    || !add_object(data, make_obj_quad(make_float3(mn.x, mn.y, mn.z), dx, dz, mat)))
    return (0);
  return (1);
}

// static cl_float3 rot_z_point(cl_float3 p, cl_float3 c, float ct, float st)
// {
//   float dx = p.x - c.x;
//   float dy = p.y - c.y;
//   return (make_float3(c.x + ct * dx - st * dy,
//                       c.y + st * dx + ct *dy, p.z));
// }

// static cl_float3 rot_z_vec(cl_float3 v, float ct, float st)
// {
//   return (make_float3(ct * v.x - st * v.y, st * v.x + ct * v.y, v.z));
// }

// void rotate_quad_z(t_object *o, cl_float3 pivot, float ct, float st)
// {
//   cl_float3 n;

//   o->center = rot_z_point(o->center, pivot, ct, st);
//   o->u = rot_z_vec(o->u, ct, st);
//   o->v = rot_z_vec(o->v, ct, st);
//   n = cross(o->u, o->v);
//   o->normal = norm(n);
//   o->d = dot(o->normal, o->center);
//   o->w = divide(n, dot(n, n));
// }

/*
** Add a ground quad and two emissive lights (one warm, one bluish) in the back
** corners, 45 deg up/behind the cube. Their indices and base offsets are saved
** so update_stage can scale them with the explosion.
*/
void  add_stage(t_data *data)
{
  float  h;

  data->scene_scale = 1.0f;
  data->stage_floor_y = data->rubik.center.y - RUBIK_STEP - RUBIK_STEP * 0.46f;
  data->light_base[0] = make_float3(STAGE_LIGHT_SPREAD, STAGE_LIGHT_HEIGHT,
      STAGE_LIGHT_BACK);
  data->light_base[1] = make_float3(-STAGE_LIGHT_SPREAD, STAGE_LIGHT_HEIGHT,
      STAGE_LIGHT_BACK);
  h = STAGE_FLOOR_HALF;
  data->stage_floor = (int)data->obj_count;
  add_object(data, make_obj_quad(make_float3(data->rubik.center.x - h,
        data->stage_floor_y, data->rubik.center.z - h),
      make_float3(2 * h, 0, 0), make_float3(0, 0, 2 * h),
      material_init(make_float3(0.45f, 0.45f, 0.48f), 0)));
  data->stage_light[0] = (int)data->obj_count;
  add_object(data, make_obj_sphere(add(data->rubik.center, data->light_base[0]),
      STAGE_LIGHT_RADIUS, material_init(make_float3(2.2f, 1.5f, 0.8f), 2)));
  data->stage_light[1] = (int)data->obj_count;
  add_object(data, make_obj_sphere(add(data->rubik.center, data->light_base[1]),
      STAGE_LIGHT_RADIUS, material_init(make_float3(0.5f, 0.9f, 2.6f), 2)));
  data->has_stage = 1;
}

/* Reposition + resize the floor and lights for the current scene_scale. */
void  update_stage(t_data *data)
{
  float  s;
  float  h;
  int    k;

  if (!data->has_stage)
    return ;
  s = data->scene_scale;
  k = 0;
  while (k < 2)
  {
    data->objects[data->stage_light[k]] = make_obj_sphere(
        add(data->rubik.center, scale(data->light_base[k], s)),
        STAGE_LIGHT_RADIUS * s,
        data->objects[data->stage_light[k]].material);
    k++;
  }
  h = STAGE_FLOOR_HALF * s;
  data->objects[data->stage_floor] = make_obj_quad(
      make_float3(data->rubik.center.x - h, data->stage_floor_y,
        data->rubik.center.z - h), make_float3(2 * h, 0, 0),
      make_float3(0, 0, 2 * h), data->objects[data->stage_floor].material);
}

void make_rubick_cube(t_data *data)
{
  // uint32_t       i;
  // uint32_t      start;
  // cl_float3     pivot;
  // float         rad;

  // start = data->obj_count;

  t_material red = material_init(make_float3(1.0f, 0.0f, 0.0f), 0);
  t_material blue = material_init(make_float3(0.0f, 0.0f, 1.0f), 0);
  // t_material white = material_init(make_float3(1.0f, 1.0f, 1.0f), 0);
  t_material green = material_init(make_float3(0.0f, 0.5f, 0.0f), 0);
  // t_material yellow = material_init(make_float3(1.0f, 1.0f, 0.0f), 0);
  // t_material orange = material_init(make_float3(1.0f, 0.647f, 0.0f), 0);

  make_box(data, make_float3(-2.0f, -2.0f, 15.0f), make_float3(2.0f, 2.0f, 19.0f), blue);
  make_box(data, make_float3(-6.1f, -2.0f, 15.0f), make_float3(-2.1f, 2.0f, 19.0f), blue);
  make_box(data, make_float3(-10.2f, -2.0f, 15.0f), make_float3(-6.2f, 2.0f, 19.0f), blue);

  make_box(data, make_float3(-2.0f, 2.0f, 15.0f), make_float3(2.0f, 6.0f, 19.0f), red);
  make_box(data, make_float3(-6.1f, 2.0f, 15.0f), make_float3(-2.1f, 6.0f, 19.0f), red);
  make_box(data, make_float3(-10.2f, 2.0f, 15.0f), make_float3(-6.2f, 6.0f, 19.0f), red);

  make_box(data, make_float3(-2.0f, -2.0f, 15.0f), make_float3(2.0f, -6.0f, 19.0f), green);
  make_box(data, make_float3(-6.1f, -2.0f, 15.0f), make_float3(-2.1f, -6.0f, 19.0f), green);
  make_box(data, make_float3(-10.2f, -2.0f, 15.0f), make_float3(-6.2f, -6.0f, 19.0f), green);

  // pivot = make_float3(-4.1f, 0.0f, 17.0f);
  // rad = degrees_to_radians(45.0f);
  // i = start;
  // while (i < data->obj_count)
  //   rotate_quad_z(&data->objects[i++], pivot, cosf(rad), sinf(rad));

  data->cam_center = make_float3(0, 0 , 0);
  data->cam_dir = make_float3(0, 0, 1);
  data->cam_fov = 70;
  data->frame.background = make_float3(0.564f, 0.835f, 1.0f);
}


/*
** The classic Cornell box (5 walls + ceiling light + 2 boxes), ported from
** make_cornell_box() in src/main_cpu.c. Also sets the camera and a black
** background so the only light is the emissive ceiling quad.
*/
void  make_cornell_box(t_data *data)
{
  t_material red = material_init(make_float3(0.65f, 0.05f, 0.05f), 0);
  t_material white = material_init(make_float3(0.73f, 0.73f, 0.73f), 0);
  t_material green = material_init(make_float3(0.12f, 0.45f, 0.15f), 0);
  t_material light = material_init(make_float3(50.0f, 50.0f, 50.0f), 2);

  add_object(data, make_obj_quad(make_float3(555, 0, 0), make_float3(0, 555, 0), make_float3(0, 0, 555), green));
  add_object(data, make_obj_quad(make_float3(0, 0, 0), make_float3(0, 555, 0), make_float3(0, 0, 555), red));
  add_object(data, make_obj_quad(make_float3(343, 554, 332), make_float3(-130, 0, 0), make_float3(0, 0, -105), light));
  // add_object(data, make_obj_quad(make_float3(555,0,0), make_float3(0, 100, 0), make_float3(0, 0, 130), light));
  add_object(data, make_obj_quad(make_float3(0, 0, 0), make_float3(555, 0, 0), make_float3(0, 0, 555), white));
  add_object(data, make_obj_quad(make_float3(555, 555, 555), make_float3(-555, 0, 0), make_float3(0, 0, -555), white));
  add_object(data, make_obj_quad(make_float3(0, 0, 555), make_float3(555, 0, 0), make_float3(0, 555, 0), white));
  make_box(data, make_float3(130, 0, 65), make_float3(295, 165, 230), white);
  make_box(data, make_float3(265, 0, 295), make_float3(430, 330, 460), white);
  data->cam_center = make_float3(278, 278, -800);
  data->cam_dir = make_float3(0, 0, 1);
  data->cam_fov = 40;
  data->frame.background = make_float3(0, 0, 0);
}

/*
** Stage props for the Rubik scene: an infinite grey floor just under the cube
** and a downward-facing emissive quad above it (the "plane light"). The cube
** bottom sits at y ~ -3.2 and its center is at (-4, 0, 17).
// */
// void  rubik_stage(t_data *data)
// {
//   (void) data;
//   // t_material  ground;
//   // t_material  lamp;

//   // ground = material_init(make_float3(0.5f, 0.5f, 0.5f), 0);
//   // lamp = material_init(make_float3(18.0f, 18.0f, 18.0f), 2);
//   // add_object(data, make_obj_plane(make_float3(0.0f, -6.3f, 0.0f),
//   //     make_float3(0.0f, 1.0f, 0.0f), ground));
//   // add_object(data, make_obj_quad(make_float3(-10.0f, 10.0f, 11.0f),
//   //     make_float3(12.0f, 0.0f, 0.0f), make_float3(0.0f, 0.0f, 12.0f), lamp));
// }
