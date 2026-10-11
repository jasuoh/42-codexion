/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 14:05:49 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 17:42:27 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
 * start_sim - sets the start time and launches all threads.
 * start_time and every last_compile_start are set before a thread runs
 * (no mutex needed yet). The monitor starts after all coders.
 * If pthread_create fails, stop is set so the running threads end.
 * monitor_init tells end_sim whether the monitor exists.
 * Returns the number of coder threads that were started.
 */
int	start_sim(t_config *config)
{
	int		i;
	t_coder	*coder;

	config->start_time = get_time_in_ms();
	i = 0;
	while (i < config->nb_of_coders)
	{
		coder = &config->coders[i];
		coder->last_compile_start = config->start_time;
		if (pthread_create(&coder->thread, NULL, normal_routine, coder))
		{
			set_stop(config);
			return (i);
		}
		i++;
	}
	if (pthread_create(&config->monitor_thread, NULL, monitor_routine, config))
		set_stop(config);
	else
		config->monitor_init = 1;
	return (i);
}

/*
 * end_sim - waits for the started coder threads and the monitor.
 * Only joins threads that really exist (coders = return of start_sim,
 * monitor_init for the monitor). Must run before cleanup, otherwise the
 * threads would use freed memory.
 */
void	end_sim(t_config *config, int coders)
{
	int	i;

	i = 0;
	while (i < coders)
		pthread_join(config->coders[i++].thread, NULL);
	if (config->monitor_init == 1)
		pthread_join(config->monitor_thread, NULL);
}
