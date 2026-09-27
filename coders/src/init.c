#include "../include/codexion.h"

int init_coders(t_config *config)
{
    int i;

    i = 0;
    while (i < config->nb_of_coders)
    {
        config->coders->start_time = get_time_in_ms();
        if (!pthread_create(&config->coders->thread, NULL, normal_routine, &config->coders))
            return (FAIL);
        i++;
    }
    return (SUCESS);
}


int wait_coders(t_config *config)
{
    int i;

    i = 0;
    while (i < config->nb_of_coders)
    {
        if (!pthread_join(config->coders[i].thread, NULL))
            return (FAIL);
        i++;
    }
    return SUCESS;
}