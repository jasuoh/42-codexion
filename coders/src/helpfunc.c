/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpfunc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:09:20 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:16:22 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

/*
 * cleanup - destroys all mutexes / condition variables and frees memory.
 * Only destroys what was initialized (see the *_ready counters), so it is
 * safe after a half finished init. Must only run after all threads joined.
 */
void	cleanup(t_config *config)
{
	int	i;

	if (!config)
		return ;
	i = 0;
	while (i < config->coders_ready)
		pthread_mutex_destroy(&config->coders[i++].state_mutex);
	free(config->coders);
	i = 0;
	while (i < config->dongles_ready)
	{
		pthread_mutex_destroy(&config->dongles[i].mutex);
		pthread_cond_destroy(&config->dongles[i].cond);
		i++;
	}
	free(config->dongles);
	if (config->mutexes_ready >= 1)
		pthread_mutex_destroy(&config->print_mutex);
	if (config->mutexes_ready >= 2)
		pthread_mutex_destroy(&config->sim_mutex);
	free(config);
}

/*
 * get_time_in_ms - current wall clock time in milliseconds since 1970.
 * Microseconds are truncated, so the value can be up to 1 ms behind.
 */
long	get_time_in_ms(void)
{
	struct timeval	ct;

	gettimeofday(&ct, NULL);
	return (ct.tv_sec * 1000 + ct.tv_usec / 1000);
}

/*
 * log_message - prints "<ms since start> <id> <action>" for a coder.
 * Holds print_mutex so two lines never mix. The timestamp is taken inside
 * the lock -> timestamps in the output are always ascending.
 * Prints nothing once stop is set -> no line after "burned out".
 * Lock order: print_mutex -> sim_mutex (via get_stop).
 */
void	log_message(t_config *config, int id, char *action)
{
	long	time;

	pthread_mutex_lock(&config->print_mutex);
	if (!get_stop(config))
	{
		time = get_time_in_ms();
		printf("%ld %d %s\n", time - config->start_time, id, action);
	}
	pthread_mutex_unlock(&config->print_mutex);
}

/*
 * get_stop - thread safe read of the global stop flag (sim_mutex).
 * The value is copied before unlocking, so the mutex is always released.
 * Returns 1 if the simulation is over, 0 otherwise.
 */
int	get_stop(t_config *config)
{
	int	stop;

	pthread_mutex_lock(&config->sim_mutex);
	stop = config->stop;
	pthread_mutex_unlock(&config->sim_mutex);
	return (stop);
}

/*
 * set_stop - ends the simulation (sim_mutex protected).
 * One way switch: stop starts at 0 (memset) and only ever becomes 1.
 * Called by the monitor (burnout / everyone done) or by start_sim when a
 * thread could not be created.
 */
void	set_stop(t_config *config)
{
	pthread_mutex_lock(&config->sim_mutex);
	config->stop = 1;
	pthread_mutex_unlock(&config->sim_mutex);
}
