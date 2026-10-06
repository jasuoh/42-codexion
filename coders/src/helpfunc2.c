/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpfunc2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 09:15:03 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/06 09:15:35 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	custom_sleep(long time, t_config *config)
{
	long	ct;
	long	nt;

	ct = get_time_in_ms();
	nt = ct + time;
	while (ct < nt)
	{
		if (get_stop(config))
			return (FAIL);
		usleep(500);
		ct = get_time_in_ms();
	}
	return (SUCESS);
}
