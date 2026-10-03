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
# include <limits.h>

# define SUCESS 1
# define FAIL 0

# define USED 1
# define FREE 0

# define FIFO 1
# define EDF 0

/* a dongle is shared by max. 2 coders -> max. 2 waiting requests */
# define HEAP_CAP 2

typedef struct s_coder		t_coder;
typedef struct s_request	t_request;
typedef struct s_heap		t_heap;
typedef struct s_config		t_config;
typedef struct s_dongle		t_dongle;

/* state_mutex protects last_compile_start + times_compiled (monitor reads) */
typedef struct s_coder
{
	pthread_t		thread;
	int				id;
	int				times_compiled;
	long			last_compile_start;
	pthread_mutex_t	state_mutex;
	t_config		*config;
	t_dongle		*left;
	t_dongle		*right;
}	t_coder;

/* seq = arrival order (fifo + tie-break), deadline for edf */
typedef struct s_request
{
	t_coder	*coder;
	long	seq;
	long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	items[HEAP_CAP];
	int			size;
}	t_heap;

/* mutex protects all fields, cond wakes waiting coders */
typedef struct s_dongle
{
	int				id;
	int				state;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			queue;
}	t_dongle;

/* *_ready = how many were initialized -> cleanup after half init */
typedef struct s_config
{
	int				nb_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	long			start_time;
	int				stop;
	long			next_seq;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	sim_mutex;
	int				mutexes_ready;
	t_coder			*coders;
	int				coders_ready;
	t_dongle		*dongles;
	int				dongles_ready;
}	t_config;

int			check_args(int argc, char **argv);
t_config	*init_config(char **argv);
int			init_all(t_config *config);
void		cleanup(t_config *config);
void		*normal_routine(void *arg);
long		get_time_in_ms(void);

#endif
