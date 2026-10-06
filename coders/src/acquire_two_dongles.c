/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_two_dongles.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 17:48:05 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:12:42 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

/*
 * wait_for_both - waits until both dongles can be taken or stop is set.
 * Called with first->mutex and second->mutex locked. timedwait can only
 * release one mutex, so second is unlocked while sleeping on first->cond
 * and locked again afterwards -> lock order first -> second is kept.
 * The 1 ms timeout also catches changes on second (no broadcast on first).
 */
static void	wait_for_both(t_coder *coder, t_dongle *first, t_dongle *second)
{
	struct timespec	wake;

	while (
		!get_stop(coder->config)
		&& !(can_get_dongle(first, coder) && can_get_dongle(second, coder))
	)
	{
		pthread_mutex_unlock(&second->mutex);
		wake = get_wakeup_time();
		pthread_cond_timedwait(&first->cond, &first->mutex, &wake);
		pthread_mutex_lock(&second->mutex);
	}
}

/*
 * take_dongle - removes the coder's own request and marks the dongle USED.
 * First in queue -> normal pop. Otherwise the coder passed someone (see
 * can_get_dongle): with HEAP_CAP 2 its request is then the last element,
 * and removing the last leaf keeps the heap valid -> size--.
 * heap_pop would wrongly remove the coder that was passed.
 * Caller must hold dongle->mutex.
 */
static void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue.items[0].coder == coder)
		heap_pop(&dongle->queue, coder->config->scheduler);
	else
		dongle->queue.size--;
	dongle->state = USED;
}

/*
 * take_both_dongles - takes both dongles at once (or none).
 * One request (same seq + deadline) goes into both queues, so all waiting
 * requests share one global order. A coder never holds one dongle while
 * waiting for the other -> Hold and Wait is broken -> no deadlock.
 * Both mutexes are locked lower dongle id first (first/second are sorted
 * by get_both_dongles) -> no deadlock between the mutexes either.
 * Returns SUCESS with both dongles USED, FAIL if stop was set while waiting.
 */
int	take_both_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_request	request;

	request = get_request(coder);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(&first->queue, coder->config->scheduler, request);
	heap_push(&second->queue, coder->config->scheduler, request);
	wait_for_both(coder, first, second);
	if (get_stop(coder->config))
	{
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		return (FAIL);
	}
	take_dongle(first, coder);
	take_dongle(second, coder);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	log_message(coder->config, coder->id, "has taken a dongle");
	log_message(coder->config, coder->id, "has taken a dongle");
	return (SUCESS);
}
