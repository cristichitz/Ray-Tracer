/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rubick_solve_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:52:15 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:52:16 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include <stdlib.h>

/*
** Push a move onto the play queue (ring buffer). When `record` is set the move
** is also appended to the history, which is the list of moves applied since
** the cube was last solved; solve_rubik replays the inverse of that list.
*/
static void	queue_move(t_rubik *r, t_move m, int record)
{
	if (r->q_count >= MAX_MOVES)
		return ;
	r->queue[(r->q_head + r->q_count) % MAX_MOVES] = m;
	r->q_count++;
	if (record && r->h_count < MAX_MOVES)
		r->history[r->h_count++] = m;
}

/*
** Queue SCRAMBLE_LEN random outer-slice turns (recorded into history).
** turns is drawn from {-1, +1, +2}: a quarter turn either way or a half turn.
*/
void	scramble_rubik(t_rubik *r)
{
	t_move	m;
	int		i;

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
void	solve_rubik(t_rubik *r)
{
	t_move	m;
	int		i;

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
