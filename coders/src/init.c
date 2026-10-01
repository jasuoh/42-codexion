#include "../include/codexion.h"

int init_coders(t_config *config)
{
    int id;

    id = 0;
    config->coders = malloc(sizeof(t_coder) * config->nb_of_coders);
    if (!config->coders)
        return (0);
    while (id < config->nb_of_coders)
    {
        config->coders[id].id = id + 1;
        config->coders[id].time = get_time_in_ms();
        config->coders[id].times_compiled = 0;
        config->coders[id].config = config;
        if (!pthread_create(&config->coders[id].thread, NULL, normal_routine, &config->coders[id].id))
            return (FAIL);
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


int init_dongles(t_config *config)
{
    return (1);
}

int init_all(t_config *config)
{
    if (!init_coders(config))
        return (FAIL);
    if (!wait_coders(config))
        return (FAIL);
    if (!init_dongles(config))
        return (FAIL);
    return (SUCESS);
}
