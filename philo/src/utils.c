/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wdaher-a <wdaher-a@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 17:00:26 by wdaher-a          #+#    #+#             */
/*   Updated: 2026/01/09 12:43:50 by wdaher-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

int     log_status(t_state *state, int id, char *status)
{
        long long   current_time;

        if (pthread_mutex_lock(&state->write_mutex) != 0)
        {
            return (1);
        }
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
    int i = 0;
    int sign = 1;
    long long result = 0;

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
        if (result > INT_MAX)
        i++;
    }

    result *= sign;

    if (result > INT_MAX)
        return INT_MAX;
    if (result < INT_MIN)
        return INT_MIN;

    return (int)result;
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