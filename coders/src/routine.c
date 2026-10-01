#include "../include/codexion.h"

int coder_compile(t_coder *coder)
{
    
}

int coder_debug(t_coder *coder)
{

}

int coder_refactor(t_coder *coder)
{
    
}




void *normal_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    if (!coder)
        return NULL;
    while (coder->times_compiled < coder->config->number_of_compiles_required)
    {
        if (!coder_compile(coder))
            return (0);
        coder->times_compiled++;
    }
}