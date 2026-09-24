/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argparse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:41:34 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 21:08:39 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	is_digit(char *str)
{
	size_t	i;

	i = 0;
	while (i < strlen(str))
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}

int	check_args(int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 9)
		return (0);
	while (i < argc - 1)
	{
		if (is_digit(argv[i]))
			i++;
		else
			return (0);
	}
	if ((strcmp(argv[8], "fifo") != 0) && (strcmp(argv[8], "edf") != 0))
		return (0);
	return (1);
}

/*
Iniializing the config the the passed args on the CL
*/

t_config	*init_config(char **argv)
{
	t_config	*config;

	config = malloc(sizeof(t_config));
	if (!config)
		return (NULL);
	config->nb_of_coders = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->number_of_compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = malloc(sizeof(char) * strlen(argv[8]));
	if (!config->scheduler)
		return (free(config), NULL);
	return (config);
}
