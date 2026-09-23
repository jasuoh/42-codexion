/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argparse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:41:34 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 15:08:16 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	is_digit(char *str)
{
	int	i;

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

	if (argc != 9)
		return (0);
	i = 1;
	while (i < argc - 1)
	{
		if (is_digit(argv[i]))
			i++;
		else
			return (0);
	}
	if (!((strcmp(argv[8], "FIFO") || (strcmp(argv[8], "EDF")))))
		return (0);
	return (1);
}
	/*
	Iniializing the config the the passed args on the CL
	*/

t_config	init_config(char **argv)
{
	int	i;

	i = 1;
}

int	main(int argc, char **argv)
{
	printf("%d\n", check_args(argc, argv));
}
