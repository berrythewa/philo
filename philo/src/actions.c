#include "../includes/philosophers.h"

int eat(t_philosopher *philo)
{

    pthread_mutex_lock(&philo->state->forks[philo->left_fork]);
    pthread_mutex_lock(&philo->state->forks[philo->right_fork]);
    log_status(philo->state, philo->id, "has taken a fork");
    log_status(philo->state, philo->id, "has taken a fork");

    if (log_status(philo->state, philo->id, "is eating") != 0)
        return (1);
    
    philo->last_meal_time = get_time();
    
    if (smart_sleep(philo->state->time_to_eat, philo->state) != 0)
        return (1);
    
    philo->meals_eaten++;
    
    pthread_mutex_unlock(&philo->state->forks[philo->left_fork]);
    pthread_mutex_unlock(&philo->state->forks[philo->right_fork]);
    return (0);
}


int _sleep(t_philosopher *philo)
{
    if (log_status(philo->state, philo->id, "is sleeping") != 0)
        return (1);
        
    if (smart_sleep(philo->state->time_to_sleep, philo->state) != 0)
        return (1);
    return (0);
}


int think(t_philosopher *philo)
{
    if (log_status(philo->state, philo->id, "is thinking") != 0)
        return (1);
    return (0);
}