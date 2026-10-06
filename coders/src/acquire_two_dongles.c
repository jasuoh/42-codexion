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

static void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue.items[0].coder == coder)
		heap_pop(&dongle->queue, coder->config->scheduler);
	else
		dongle->queue.size--;
	dongle->state = USED;
}

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
