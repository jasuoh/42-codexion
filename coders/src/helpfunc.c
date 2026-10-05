/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpfunc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:09:20 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 11:39:27 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

/* only destroys what was initialized (see *_ready) -> safe after half init */
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

long get_time_in_ms(void)
{
	struct timeval ct;

	gettimeofday(&ct, NULL);
	return (ct.tv_sec * 1000 + ct.tv_usec / 1000);
}

void log_message(t_config *config, int id, char *action)
{
	long time;
	pthread_mutex_lock(&config->print_mutex);
	if (!get_stop(config))
	{
		time = get_time_in_ms();
		printf("%ld %d %s\n", time - config->start_time, id, action);
	}
	pthread_mutex_unlock(&config->print_mutex);
}

int get_stop(t_config *config)
{
	int stop;
	pthread_mutex_lock(&config->sim_mutex);
	stop = config->stop;
	pthread_mutex_unlock(&config->sim_mutex);
	return stop;
}

void set_stop(t_config *config)
{
	pthread_mutex_lock(&config->sim_mutex);
	config->stop = 1;
	pthread_mutex_unlock(&config->sim_mutex);
}

int custom_sleep(long time, t_config *config)
{
	long	ct;
	long nt;
	ct = get_time_in_ms();
	nt = ct + time;
	while (ct < nt)
	{
		if (get_stop(config))
			return (FAIL);
		usleep(500);
		ct = get_time_in_ms();
	}
	return (SUCESS);
}
