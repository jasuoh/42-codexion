/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 15:45:43 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 17:47:55 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

struct	timespec	get_wakeup_time(void)
{
	struct timespec	returnval;
	long	tt;

	tt = get_time_in_ms() + 1;
	returnval.tv_sec = tt / 1000;
	returnval.tv_nsec = (tt % 1000) * 1000000;
	return (returnval);
}

/* Use only when dongle mutex is locked*/
int	can_get_dongle(t_dongle *dongle, t_coder *coder)
{
	if (
		dongle->queue.items[0].coder == coder
		&& dongle->state == FREE && get_time_in_ms() >= dongle->available_at
	)
		return (SUCESS);
	return (FAIL);
}

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

void	return_dongle(t_dongle *dongle, t_config *config)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->state = FREE;
	dongle->available_at = get_time_in_ms() + config->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
