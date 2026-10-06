/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 15:45:43 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:13:01 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

/*
 * get_wakeup_time - absolute time "now + 1 ms" for pthread_cond_timedwait.
 * timedwait needs an absolute struct timespec (seconds + nanoseconds), so
 * the ms value is split: tv_sec = ms / 1000, tv_nsec = rest in ns.
 * Waking up every 1 ms lets a waiting coder notice the end of a cooldown
 * and the stop flag, two events nobody broadcasts.
 */
struct	timespec	get_wakeup_time(void)
{
	struct timespec	returnval;
	long			tt;

	tt = get_time_in_ms() + 1;
	returnval.tv_sec = tt / 1000;
	returnval.tv_nsec = (tt % 1000) * 1000000;
	return (returnval);
}

/*
 * can_get_dongle - may this coder take this dongle right now?
 * The dongle must be FREE and out of cooldown. Then the coder may take it
 * if its request is first in the queue (FIFO / EDF order), or if the first
 * one still has enough time: the dongle would be back (compile + cooldown
 * + BYPASS_MARGIN) before the deadline of the first one. This avoids
 * convoys where a free dongle waits for someone who is blocked elsewhere,
 * while nobody can be passed once their deadline gets close.
 * Caller must hold dongle->mutex. Returns 1 / SUCESS or 0 / FAIL.
 */
int	can_get_dongle(t_dongle *dongle, t_coder *coder)
{
	t_config	*config;
	long		now;
	long		back_at;

	config = coder->config;
	now = get_time_in_ms();
	if (dongle->state == USED || now < dongle->available_at)
		return (FAIL);
	if (dongle->queue.items[0].coder == coder)
		return (SUCESS);
	back_at = now + config->time_to_compile + config->dongle_cooldown;
	return (dongle->queue.items[0].deadline > back_at + BYPASS_MARGIN);
}

/*
 * get_request - builds a queue entry for this coder.
 * seq: unique ticket number from next_seq, read + incremented in one
 * sim_mutex section so no two requests get the same number.
 * deadline: last_compile_start + time_to_burnout (used by EDF and by the
 * bypass check). last_compile_start is read without state_mutex because
 * only this coder's own thread ever writes it.
 */
t_request	get_request(t_coder *coder)
{
	t_request	request;
	long		deadline;

	request.coder = coder;
	pthread_mutex_lock(&coder->config->sim_mutex);
	request.seq = coder->config->next_seq;
	coder->config->next_seq++;
	pthread_mutex_unlock(&coder->config->sim_mutex);
	deadline = coder->last_compile_start + coder->config->time_to_burnout;
	request.deadline = deadline;
	return (request);
}

/*
 * acquire_dongle - takes a single dongle (only used with 1 coder).
 * Queues a request, waits on the condition variable until the dongle can
 * be taken or stop is set, then removes the request and marks it USED.
 * The log line is printed after unlocking to keep the critical section
 * short. Returns SUCESS, or FAIL if the simulation stopped while waiting.
 */
int	acquire_dongle(t_config *config, t_dongle *dongle, t_coder *coder)
{
	struct timespec	waiting_time;

	pthread_mutex_lock(&dongle->mutex);
	if (!heap_push(&dongle->queue, config->scheduler, get_request(coder)))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (FAIL);
	}
	while (!get_stop(config) && !(can_get_dongle(dongle, coder)))
	{
		waiting_time = get_wakeup_time();
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &waiting_time);
	}
	if (get_stop(config))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (FAIL);
	}
	heap_pop(&dongle->queue, config->scheduler);
	dongle->state = USED;
	pthread_mutex_unlock(&dongle->mutex);
	log_message(config, coder->id, "has taken a dongle");
	return (SUCESS);
}

/*
 * return_dongle - puts a dongle back on the table.
 * Marks it FREE, starts the cooldown (available_at) and wakes every coder
 * waiting on it. broadcast, not signal: the woken coder might not be the
 * one allowed to take it. State is changed before the broadcast, all under
 * the dongle mutex -> no lost wakeup.
 */
void	return_dongle(t_dongle *dongle, t_config *config)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->state = FREE;
	dongle->available_at = get_time_in_ms() + config->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
