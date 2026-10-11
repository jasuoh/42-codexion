/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 16:58:22 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:18:04 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
 * write_burnout - prints "<ms> <id> burned out" and sets stop atomically.
 * Both happen while print_mutex is held, so no coder can print in between,
 * and every coder that gets print_mutex afterwards sees stop and stays
 * silent. Uses printf directly: log_message would lock print_mutex a
 * second time (self deadlock) and stay silent once stop is set.
 * Lock order print_mutex -> sim_mutex, same as in log_message.
 */
void	write_burnout(t_coder *coder)
{
	long	time;

	pthread_mutex_lock(&coder->config->print_mutex);
	if (!get_stop(coder->config))
	{
		time = get_time_in_ms();
		printf(
			"%ld %d %s",
			time - coder->config->start_time, coder->id, "burned out\n"
			);
		set_stop(coder->config);
	}
	pthread_mutex_unlock(&coder->config->print_mutex);
}

/*
 * check_burnout - has this coder not started compiling for
 * time_to_burnout ms? last_compile_start is copied under state_mutex
 * (written by the coder thread, read here by the monitor thread).
 * Returns 1 if burned out, 0 otherwise.
 */
int	check_burnout(t_coder *coder)
{
	long	last;

	pthread_mutex_lock(&coder->state_mutex);
	last = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	return (get_time_in_ms() - last >= coder->config->time_to_burnout);
}

/*
 * all_done - has every coder compiled at least number_of_compiles_required
 * times? Each counter is copied under its state_mutex, the mutex is
 * released before comparing. Returns SUCESS if all are done, else FAIL.
 */
int	all_done(t_config *config)
{
	int		i;
	t_coder	*coder;
	int		nb_times_compiled;

	i = 0;
	while (i < config->nb_of_coders)
	{
		coder = &config->coders[i];
		pthread_mutex_lock(&coder->state_mutex);
		nb_times_compiled = coder->times_compiled;
		pthread_mutex_unlock(&coder->state_mutex);
		if (nb_times_compiled < config->number_of_compiles_required)
			return (FAIL);
		i++;
	}
	return (SUCESS);
}

/*
 * monitor_routine - thread function of the monitor (arg = t_config).
 * Every ~0.5 ms: check every coder for burnout (-> report + stop), then
 * check if everyone is done (-> stop, no message). The short sleep keeps
 * the burnout log well within the required 10 ms without busy waiting.
 */
void	*monitor_routine(void *arg)
{
	t_config	*config;
	int			i;

	config = (t_config *)arg;
	while (!get_stop(config))
	{
		i = 0;
		while (i < config->nb_of_coders)
		{
			if (check_burnout(&config->coders[i]))
			{
				write_burnout(&config->coders[i]);
				return (NULL);
			}
			i++;
		}
		if (all_done(config))
		{
			set_stop(config);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}
