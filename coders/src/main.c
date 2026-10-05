/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:19:19 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 14:53:05 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char **argv)
{
	t_config	*config;
	int	coders;
	int nb_of_coders;

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
	end_sim(config, coders);
	nb_of_coders = config->nb_of_coders;
	cleanup(config);
	if (coders != nb_of_coders)
		return (1);
	return (0);
}
