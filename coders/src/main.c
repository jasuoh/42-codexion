/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:19:19 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:27:53 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
 * main - parse arguments -> init -> start threads -> join -> cleanup.
 * Returns 0 on a normal run (burnout included), 1 on invalid arguments
 * or when initialization / thread creation failed.
 */
int	main(int argc, char **argv)
{
	t_config	*config;
	int			coders;
	int			failed;

	if (!check_args(argc, argv))
		return (1);
	config = init_config(argv);
	if (!config)
		return (1);
	if (!init_all(config))
	{
		write(2, "Error: initialization failed\n", 29);
		cleanup(config);
		return (1);
	}
	coders = start_sim(config);
	failed = (coders != config->nb_of_coders || !config->monitor_init);
	end_sim(config, coders);
	cleanup(config);
	return (failed);
}
