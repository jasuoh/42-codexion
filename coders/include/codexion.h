/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:19:09 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 15:14:07 by jawosylu         ###   ########.fr       */
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

typedef struct s_config
{
	int		nb_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	*scheduler;

}	t_config;

typedef struct s_coder
{
	pthread_t	thread;
	int			id;
	int			times_compiled;
	size_t		time_to_burnout;
	size_t		time_to_compile;
	size_t		time_to_refactor;
	size_t		start_time;

}	t_coder;

int	check_args(int argc, char **argv);

#endif 