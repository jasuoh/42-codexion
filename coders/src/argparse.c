/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argparse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:41:34 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 15:32:23 by jawosylu         ###   ########.fr       */
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
	
}
