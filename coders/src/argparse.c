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

static int	print_error(char *msg)
{
	write(2, "Error: ", 7);
	write(2, msg, strlen(msg));
	write(2, "\n", 1);
	return (FAIL);
}

/* only digits, not empty, no overflow over INT_MAX */
static int	parse_number(char *str, int *out)
{
	long	nb;
	int		i;

	if (!str[0])
		return (FAIL);
	nb = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (FAIL);
		nb = nb * 10 + (str[i] - '0');
		if (nb > INT_MAX)
			return (FAIL);
		i++;
	}
	*out = (int)nb;
	return (SUCESS);
}

int	check_args(int argc, char **argv)
{
	int	i;
	int	nb;

	if (argc != 9)
		return (print_error("expected 8 arguments"));
	i = 1;
	while (i <= 7)
	{
		if (!parse_number(argv[i], &nb))
			return (print_error("arguments 1-7 must be integers 0..INT_MAX"));
		if ((i == 1 || i == 6) && nb == 0)
			return (print_error("coders and compiles required must be > 0"));
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (print_error("scheduler must be 'fifo' or 'edf'"));
	return (SUCESS);
}

/* only call after check_args succeeded */
t_config	*init_config(char **argv)
{
	t_config	*config;

	config = malloc(sizeof(t_config));
	if (!config)
		return (NULL);
	memset(config, 0, sizeof(t_config));
	parse_number(argv[1], &config->nb_of_coders);
	parse_number(argv[2], &config->time_to_burnout);
	parse_number(argv[3], &config->time_to_compile);
	parse_number(argv[4], &config->time_to_debug);
	parse_number(argv[5], &config->time_to_refactor);
	parse_number(argv[6], &config->number_of_compiles_required);
	parse_number(argv[7], &config->dongle_cooldown);
	if (strcmp(argv[8], "fifo") == 0)
		config->scheduler = FIFO;
	else
		config->scheduler = EDF;
	return (config);
}
