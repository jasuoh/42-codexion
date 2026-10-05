/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 16:58:22 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 17:31:36 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

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

int	check_burnout(t_coder *coder)
{
	long	last;

	pthread_mutex_lock(&coder->state_mutex);
	last = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	return (get_time_in_ms() - last >= coder->config->time_to_burnout);
}

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

void	*monitor_routine(void *arg)
{
	t_coder		*coder;
	t_config	*config;
	int			i;

	config = (t_config *)arg;
	while (!get_stop(config))
	{
		i = 0;
		while (i < config->nb_of_coders)
		{
			coder = &config->coders[i];
			if (check_burnout(coder))
			{
				write_burnout(coder);
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
