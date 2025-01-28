#include "../includes/philosophers.h"

int take_forks(t_philosopher *philo)
{
    if (philo->id % 2 == 0) {
        if (pthread_mutex_lock(&philo->state->forks[philo->right_fork]) != 0)
            return (1);
        print_status(philo->state, philo->id, "has taken a fork");
        if (pthread_mutex_lock(&philo->state->forks[philo->left_fork]) != 0) {
            pthread_mutex_unlock(&philo->state->forks[philo->right_fork]);
            return (1);
        }
    } else {
        if (pthread_mutex_lock(&philo->state->forks[philo->left_fork]) != 0)
            return (1);
        print_status(philo->state, philo->id, "has taken a fork");
        if (pthread_mutex_lock(&philo->state->forks[philo->right_fork]) != 0) {
            pthread_mutex_unlock(&philo->state->forks[philo->left_fork]);
            return (1);
        }
    }
    print_status(philo->state, philo->id, "has taken a fork");
    return (0);
}

int eat(t_philosopher *philo)
{
    if (pthread_mutex_lock(&philo->state->write_mutex) != 0)
        return (1);
    print_status(philo->state, philo->id, "is eating");
    philo->state->current_actions[philo->id - 1] = EATING;
    philo->last_meal_time = get_time();
    if (pthread_mutex_unlock(&philo->state->write_mutex) != 0)
        return (1);
    
    if (smart_sleep(philo->state->time_to_eat, philo->state) != 0) {
        pthread_mutex_unlock(&philo->state->forks[philo->left_fork]);
        pthread_mutex_unlock(&philo->state->forks[philo->right_fork]);
        return (1);
    }
    
    if (pthread_mutex_lock(&philo->state->write_mutex) != 0)
        return (1);
    philo->meals_eaten++;
    if (pthread_mutex_unlock(&philo->state->write_mutex) != 0)
        return (1);
    
    if (pthread_mutex_unlock(&philo->state->forks[philo->left_fork]) != 0 ||
        pthread_mutex_unlock(&philo->state->forks[philo->right_fork]) != 0)
        return (1);
    return (0);
}

int _sleep(t_philosopher *philo)
{
    if (pthread_mutex_lock(&philo->state->write_mutex) != 0)
        return (1);
    print_status(philo->state, philo->id, "is sleeping");
    philo->state->current_actions[philo->id - 1] = SLEEPING;
    if (pthread_mutex_unlock(&philo->state->write_mutex) != 0)
        return (1);

    if (smart_sleep(philo->state->time_to_sleep, philo->state) != 0)
        return (1);
    return (0);
}

int think(t_philosopher *philo)
{
    if (pthread_mutex_lock(&philo->state->write_mutex) != 0)
        return (1);
    print_status(philo->state, philo->id, "is thinking");
    philo->state->current_actions[philo->id - 1] = THINKING;
    if (pthread_mutex_unlock(&philo->state->write_mutex) != 0)
        return (1);
    return (0);
}