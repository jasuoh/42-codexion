/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 11:19:19 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 21:08:53 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char **argv)
{
	t_config	*config;

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
	cleanup(config);
	return (0);
}
