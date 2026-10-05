/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 12:00:00 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 15:38:23 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_in_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	log_message(coder->config, coder->id, "is compiling");
	if (!custom_sleep(coder->config->time_to_compile, coder->config))
		return (FAIL);
	pthread_mutex_lock(&coder->state_mutex);
		coder->times_compiled++;
	pthread_mutex_unlock(&coder->state_mutex);
	return (SUCESS);
}

int	coder_debug(t_coder *coder)
{
	log_message(coder->config, coder->id, "is debugging");
	if (!custom_sleep(coder->config->time_to_debug, coder->config))
		return (FAIL);
	return (SUCESS);
}

int	coder_refactor(t_coder *coder)
{
	log_message(coder->config, coder->id, "is refactoring");
	if (!custom_sleep(coder->config->time_to_refactor, coder->config))
		return (FAIL);
	return (SUCESS);
}

void	*normal_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (!coder)
		return (NULL);
	while (coder->times_compiled < coder->config->number_of_compiles_required)
	{
		if (!coder_compile(coder))
			return (FAIL);
		if (!coder_debug(coder))
			return (FAIL);
		if (!coder_refactor(coder))
			return (FAIL);
	}
	return (NULL);
}
