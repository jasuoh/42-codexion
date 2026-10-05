/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 14:05:49 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 14:21:17 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int start_sim(t_config *config)
{
  int i;
  t_coder *coder;

  config->start_time = get_time_in_ms();
  i = 0;
  while (i < config->nb_of_coders)
  {
    coder = &config->coders[i];
    coder->last_compile_start = config->start_time;
    if (pthread_create(&coder->thread, NULL, normal_routine, coder))
    {
      set_stop(config);
      return (i);
    }
    i++;
  }
  return (i);
}

void end_sim(t_config *config, int coders)
{
  int i;

  i = 0;
  while (i < coders)
    pthread_join(config->coders[i++].thread, NULL);
}
