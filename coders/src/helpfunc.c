/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpfunc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:09:20 by jawosylu          #+#    #+#             */
/*   Updated: 2026/09/23 21:13:23 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	free_and_exit(t_config *config)
{
	if (*config.scheduler)
		free(config.scheduler);
	if (config)
		free(config);
}


int main(void)
{
	printf("Hello, World!\n");
}