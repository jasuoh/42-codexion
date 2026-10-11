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

#include "codexion.h"

/*
 * init_mutexes - initializes the two global mutexes.
 * print_mutex serializes the output, sim_mutex protects stop and next_seq.
 * mutexes_ready counts the successful ones so cleanup destroys only those.
 * Returns SUCESS or FAIL.
 */
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

/*
 * init_dongles - allocates one dongle per coder (ids 1..n) and initializes
 * its mutex and condition variable. Every dongle starts FREE with an empty
 * queue and available_at 0 (= available immediately).
 * dongles_ready counts fully initialized dongles for a safe cleanup.
 * Returns SUCESS or FAIL.
 */
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

/*
 * init_coders - allocates the coders (ids 1..n) and wires them to dongles.
 * Array index i: left = dongles[i], right = dongles[i + 1], the last coder
 * wraps around to dongles[0] -> coder 1 sits next to coder n.
 * With only 1 coder there is only 1 dongle on the table: right stays NULL.
 * coders_ready counts initialized state_mutexes for a safe cleanup.
 * Returns SUCESS or FAIL.
 */
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

/*
 * init_all - initializes mutexes, dongles and coders in this order.
 * Order matters: coders store pointers to dongles -> dongles first.
 * On FAIL the caller still calls cleanup, which only frees and destroys
 * what was set up (see the *_ready counters).
 * Returns SUCESS or FAIL.
 */
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
