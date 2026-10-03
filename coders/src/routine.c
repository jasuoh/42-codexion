/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 12:00:00 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/03 12:00:00 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	coder_compile(t_coder *coder)
{
	(void)coder;
	return (SUCESS);
}

int	coder_debug(t_coder *coder)
{
	(void)coder;
	return (SUCESS);
}

int	coder_refactor(t_coder *coder)
{
	(void)coder;
	return (SUCESS);
}

void	*normal_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (!coder)
		return (NULL);
	while (coder->times_compiled < coder->config->number_of_compiles_required)
	{
		if (!coder_compile(coder))
			return (0);
		coder->times_compiled++;
	}
	return (NULL);
}
