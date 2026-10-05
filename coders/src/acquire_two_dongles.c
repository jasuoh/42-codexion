/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_two_dongles.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 17:48:05 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 18:03:42 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int take_both_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	t_request request;
	struct timespec wake;

	request = get_request(coder);
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	heap_push(&first->queue, coder->config->scheduler, request);
	heap_push(&second->queue, coder->config->scheduler, request);
	while (!get_stop(coder->config) && !(can_get_dongle(first, coder) && can_get_dongle(second, coder)))
	{
		pthread_mutex_unlock(&second->mutex);
		wake = get_wakeup_time();
		pthread_cond_timedwait(&first->cond, &first->mutex, &wake);
		pthread_mutex_lock(&second->mutex);
	}
	if (get_stop(coder->config))
	{
		pthread_mutex_unlock(&first->mutex);
		pthread_mutex_unlock(&second->mutex);
		return (FAIL);
	}
	heap_pop(&first->queue, coder->config->scheduler);
	heap_pop(&second->queue, coder->config->scheduler);
	first->state = USED;
	second->state = USED;
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	log_message(coder->config, coder->id, "has taken a dongle");
	log_message(coder->config, coder->id, "has taken a dongle");
	return (SUCESS);
}
