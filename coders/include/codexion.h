/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:19:09 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 21:07:21 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

# define SUCESS 1
# define FAIL 0

# define USED 1
# define FREE 0

# define FIFO 1
# define EDF 0

typedef struct s_coder t_coder;
typedef struct s_request t_request;
typedef struct s_config t_config;
typedef struct s_dongle t_dongle;

typedef struct s_coder
{
	pthread_t	thread;
	int			id;
	int			times_compiled;
	size_t		time;
	t_config *config;
	t_dongle *left;
	t_dongle *right;

}	t_coder;

typedef struct s_request
{
	t_coder *coder;
	size_t arrival_time;

}	t_request;

typedef struct s_dongle
{
	int	id;
	int available;
	pthread_mutex_t mutex_dongle;
	int cooldown;
	t_request requests[2];

}	t_dongle;


typedef struct s_config
{
	int		nb_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	int		scheduler;
	t_coder *coders;
	t_dongle *dongles;

}	t_config;


int			check_args(int argc, char **argv);
t_config	*init_config(char **argv);
void		free_and_exit(t_config *config);
void		*normal_routine(void *arg);
long		get_time_in_ms(void);

#endif
