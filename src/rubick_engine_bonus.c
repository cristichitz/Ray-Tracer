#include "rt_bonus.h"
#include <stdlib.h>

/*
** Push a move onto the play queue (ring buffer). When `record` is set the move
** is also appended to the history, which is the list of moves applied since
** the cube was last solved; solve_rubik replays the inverse of that list.
*/
static void  queue_move(t_rubik *r, t_move m, int record)
{
  if (r->q_count >= MAX_MOVES)
    return ;
  r->queue[(r->q_head + r->q_count) % MAX_MOVES] = m;
  r->q_count++;
  if (record && r->h_count < MAX_MOVES)
    r->history[r->h_count++] = m;
}

/*
** Pop the next queued move and set up its animation: MOVE_FRAMES frames, each
** advancing the slice by an equal slice of the total angle.
*/
static void  start_move(t_rubik *r)
{
  r->current = r->queue[r->q_head];
  r->q_head = (r->q_head + 1) % MAX_MOVES;
  r->q_count--;
  r->frames_left = MOVE_FRAMES;
  r->step = degrees_to_radians(90.0f * r->current.turns) / MOVE_FRAMES;
  r->active = 1;
}

/*
** One frame of the move engine. Call it every frame before render_frame:
** advances the current turn, or starts the next queued one when idle.
*/
void  step_rubik(t_data *data)
{
  t_rubik  *r;

  r = &data->rubik;
  if (r->explode_active)
  {
    if (r->q_count == 0)
    {
      data->rubik.explode_active = 0;
      return ;
    }
    explode_rubik(r, data->objects, data);
    return ;
  }
  if (!r->active)
  {
    if (r->q_count == 0)
      return ;
    start_move(r);
  }
  rotate_layer(r, data->objects, r->step);
  r->frames_left--;
  if (r->frames_left == 0)
  {
    snap_layer(r);
    r->active = 0;
  }
}

void  explode_rubik(t_rubik *r, t_object *objs, t_data *data)  
{
  int c;
  int q;
  cl_float3 dir;
  cl_float3 delta_pos;
  cl_float3 true_center;
  float     t_prev;
  float     t_curr;
  float     dist_prev;
  float     dist_curr;
  float     pos_delta;
  float     cam_speed;
  t_object  *o;
  cl_float3 rel_cam_pos;

  int total_frames = 120;
  int current_frame = total_frames - r->q_count;
  t_prev = (float)(current_frame) / total_frames;
  t_curr = (float)(current_frame + 1) / total_frames;
  float max_dist = 15.0f;
  dist_prev = max_dist * sqrtf(sinf((float)CL_M_PI * t_prev));
  dist_curr = max_dist * sqrtf(sinf((float)CL_M_PI * t_curr));
  pos_delta = dist_curr - dist_prev;

  c = 0;
  while (c < CUBIES)
  {
    true_center = make_float3(0, 0, 0);
    q = 0;
    while (q < FACES)
    {
      true_center = add(true_center, objs[r->cubies[c].obj + q].center);
      q++;
    }
    true_center.x /= FACES;
    true_center.y /= FACES;
    true_center.z /= FACES;

    // 2. Add epsilon to prevent division by zero on the center core piece
    true_center.x += 1e-8f;
    true_center.y += 1e-8f;
    true_center.z += 1e-8f;

    dir = norm(make_float3(r->cubies[c].pos[0], r->cubies[c].pos[1], r->cubies[c].pos[2]));
    delta_pos = scale(dir, pos_delta);
    q = 0;
    while (q < FACES)
    {
      o = &objs[r->cubies[c].obj + q];
      o->center = add(o->center, delta_pos);
      o->d = dot(o->normal, o->center);
      q++;
    }
    c++;
  }
  cam_speed = 0.002f + 0.05f * fabsf(cosf((float)CL_M_PI * t_curr));

  // Rotate camera around the cube's center
  rel_cam_pos = sub(data->cam_center, data->rubik.center);
  rot_vec_axis(&rel_cam_pos, 1, cosf(cam_speed), sinf(cam_speed));
  data->cam_center = add(data->rubik.center, rel_cam_pos);

  // Make camera look at the cube's center and update yaw/pitch
  data->cam_dir = norm(sub(data->rubik.center, data->cam_center));
  data->cam_yaw = atan2f(data->cam_dir.x, data->cam_dir.z);
  data->cam_pitch = asinf(fmaxf(-1.0f, fminf(1.0f, data->cam_dir.y)));
  r->q_count -= 1;
}

/*
** Queue SCRAMBLE_LEN random outer-slice turns (recorded into history).
** turns is drawn from {-1, +1, +2}: a quarter turn either way or a half turn.
*/
void  scramble_rubik(t_rubik *r)
{
  t_move  m;
  int     i;

  i = 0;
  while (i++ < SCRAMBLE_LEN)
  {
    m.axis = rand() % 3;
    m.layer = (rand() % 2) * 2 - 1;
    m.turns = (rand() % 3) - 1;
    if (m.turns == 0)
      m.turns = 2;
    queue_move(r, m, 1);
  }
}

/*
** Queue the inverse of every recorded move, newest first, so the cube returns
** to solved. The inverses are not recorded (record = 0); clearing the history
** afterwards leaves the cube in the solved, empty-history state.
*/
void  solve_rubik(t_rubik *r)
{
  t_move  m;
  int     i;

  i = r->h_count;
  while (i-- > 0)
  {
    m = r->history[i];
    if (m.turns != 2)
      m.turns = -m.turns;
    queue_move(r, m, 0);
  }
  r->h_count = 0;
}
