/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 12:00:00 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/03 12:00:00 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	init_mutexes(t_config *config)
{
	if (pthread_mutex_init(&config->print_mutex, NULL) != 0)
		return (FAIL);
	config->mutexes_ready = 1;
	if (pthread_mutex_init(&config->sim_mutex, NULL) != 0)
		return (FAIL);
	config->mutexes_ready = 2;
	return (SUCESS);
}

static int	init_dongles(t_config *config)
{
	t_dongle	*dongle;

	config->dongles = malloc(sizeof(t_dongle) * config->nb_of_coders);
	if (!config->dongles)
		return (FAIL);
	memset(config->dongles, 0, sizeof(t_dongle) * config->nb_of_coders);
	while (config->dongles_ready < config->nb_of_coders)
	{
		dongle = &config->dongles[config->dongles_ready];
		dongle->id = config->dongles_ready + 1;
		dongle->state = FREE;
		if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
			return (FAIL);
		if (pthread_cond_init(&dongle->cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongle->mutex);
			return (FAIL);
		}
		config->dongles_ready++;
	}
	return (SUCESS);
}

/* coder i: left = dongle i, right = dongle i+1 (last one wraps to 0) */
/* only 1 coder -> only 1 dongle on the table, right stays NULL */
static int	init_coders(t_config *config)
{
	t_coder	*coder;
	int		i;

	config->coders = malloc(sizeof(t_coder) * config->nb_of_coders);
	if (!config->coders)
		return (FAIL);
	memset(config->coders, 0, sizeof(t_coder) * config->nb_of_coders);
	while (config->coders_ready < config->nb_of_coders)
	{
		i = config->coders_ready;
		coder = &config->coders[i];
		coder->id = i + 1;
		coder->config = config;
		coder->left = &config->dongles[i];
		if (config->nb_of_coders > 1)
			coder->right = &config->dongles[(i + 1) % config->nb_of_coders];
		if (pthread_mutex_init(&coder->state_mutex, NULL) != 0)
			return (FAIL);
		config->coders_ready++;
	}
	return (SUCESS);
}

/* order matters: coders point to dongles -> dongles first */
int	init_all(t_config *config)
{
	if (!init_mutexes(config))
		return (FAIL);
	if (!init_dongles(config))
		return (FAIL);
	if (!init_coders(config))
		return (FAIL);
	return (SUCESS);
}
