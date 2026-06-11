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
    explode_step(data);
    return ;
  }
  if (!r->active)
  {
    if (r->q_count == 0)
    {
      r->orbit_active = 0;
      return ;
    }
    start_move(r);
  }
  rotate_layer(r, data->objects, r->step);
  r->frames_left--;
  if (r->frames_left == 0)
  {
    snap_layer(r);
    r->active = 0;
  }
  if (r->orbit_active)
    explode_orbit_solve(data);
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
