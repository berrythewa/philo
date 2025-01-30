#include "../includes/philosophers.h"


void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    // Special case for single philosopher
    if (philo->state->num_philosophers == 1)
    {
        log_status(philo->state, philo->id, "has taken a fork");
        while (!philo->state->someone_died)
            smart_sleep(1, philo->state);
        return (NULL);
    }

    // Even philosophers start first
    if (philo->id % 2 == 1)
        smart_sleep(philo->state->time_to_eat, philo->state);  // Odd philosophers wait for even ones to get started

    while (!philo->state->someone_died && !philo->state->all_ate)
    {
        think(philo);
        
        // Skip eating if we've reached our meal limit
        if (philo->state->num_meals != -1 && philo->meals_eaten >= philo->state->num_meals)
            continue;
        
        if (philo->state->someone_died || philo->state->all_ate)
        {
            // Release forks if we have them (in case we were eating when someone died)
            pthread_mutex_unlock(&philo->state->forks[philo->left_fork]);
            pthread_mutex_unlock(&philo->state->forks[philo->right_fork]);
            break;
        }
            
        if (eat(philo) == 0)  // Only sleep if eating was successful
            _sleep(philo);
    }
    return (NULL);
}