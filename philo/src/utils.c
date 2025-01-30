#include "../includes/philosophers.h"

int log_status(t_state *state, int id, char *status)
{
    long long current_time;

    if (pthread_mutex_lock(&state->write_mutex) != 0)
        return (1);
        
    if (!state->someone_died)
    {
        current_time = get_time() - state->start_time;
        printf("%lld %d %s\n", current_time, id, status);
        fflush(stdout);
    }
    
    if (pthread_mutex_unlock(&state->write_mutex) != 0)
        return (1);
    return (0);
}


int ft_atoi(const char *str)
{
    int i;
    int sign;
    int result;

    i = 0;
    sign = 1;
    result = 0;
    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            sign = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return (sign * result);
}

int ft_atoi_lib(const char *str)
{
    size_t                          i;
    long long int           neg;
    unsigned long int       nbr;

    nbr = 0;
    neg = 0;
    i = 0;
    while ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\v') ||
            (str[i] == ' ') || (str[i] == '\f') || (str[i] == '\r'))
        ++i;
    if (str[i] == '-')
        neg = 1;
    if (str[i] == '+' || str[i] == '-')
        ++i;
    while (str[i] && (str[i] >= '0' && str[i] <= '9'))
    {
        nbr *= 10;
        nbr += (int)str[i] - '0';
        ++i;
        if (nbr > 9223372036854775807)
                return ((neg) ? 0 : -1);
    }
    if (neg)
            nbr = -1 * nbr;
    return (nbr);
}

void free_resources(t_state *state)
{
    int i;

    if (state->forks)
    {
        for (i = 0; i < state->num_philosophers; i++)
            pthread_mutex_destroy(&state->forks[i]);
        free(state->forks);
    }
    pthread_mutex_destroy(&state->write_mutex);
    if (state->philosophers)
        free(state->philosophers);
}