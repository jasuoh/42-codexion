/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 12:00:00 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 16:51:58 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int get_both_dongles(t_coder *coder, t_config *config)
{
	t_dongle *first;
	t_dongle *second;

	first = coder->left;
	second = coder->right;
	if (coder->right == NULL)
	{
		acquire_dongle(config, coder->left, coder);
		custom_sleep(999999999, config);
		return_dongle(coder->left, config);
		return (FAIL);
	}
	if (coder->right->id < coder->left->id)
	{
		first = coder->right;
		second = coder->left;
	}
	if (!acquire_dongle(config, first, coder))
		return (FAIL);
	if (!acquire_dongle(config, second, coder))
	{
		return_dongle(first, config);
		return (FAIL);
	}
	return (SUCESS);
}

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
		if (!get_both_dongles(coder, coder->config))
			return (NULL);
		if (!coder_compile(coder))
		{
			return_dongle(coder->left, coder->config);
			return_dongle(coder->right, coder->config);
			return (NULL);
		}
		return_dongle(coder->left, coder->config);
		return_dongle(coder->right, coder->config);
		if (!coder_debug(coder))
			return (NULL);
		if (!coder_refactor(coder))
			return (NULL);
	}
	return (NULL);
}
