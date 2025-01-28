#include "../includes/philosophers.h"

static int check_meals(t_philosopher *philo)
{
    if (philo->state->num_meals != -1 && philo->meals_eaten >= philo->state->num_meals)
    {
        pthread_mutex_lock(&philo->state->write_mutex);
        philo->state->finished_eating++;
        if (philo->state->finished_eating == philo->state->num_philosophers)
            philo->state->all_ate = 1;
        pthread_mutex_unlock(&philo->state->write_mutex);
        return (1);
    }
    return (0);
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo;

    philo = (t_philosopher *)arg;
    philo->last_meal_time = get_time();

    if (philo->id % 2 == 0)
        usleep(1000);  // Stagger even-numbered philosophers

    while (!philo->state->someone_died && !philo->state->all_ate)
    {
        if (check_meals(philo))
            break;
        take_forks(philo);
        if (philo->state->someone_died || philo->state->all_ate)
            break;
        eat(philo);
        if (philo->state->someone_died || philo->state->all_ate)
            break;
        if (_sleep(philo) != 0)
            break;
        if (philo->state->someone_died || philo->state->all_ate)
            break;
        if (think(philo) != 0)
            break;
    }

    return (NULL);
}