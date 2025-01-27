#include "../includes/philosophers.h"

void take_forks(t_philosopher *philo)
{
    pthread_mutex_lock(&philo->state->forks[philo->left_fork]);
    print_status(philo->state, philo->id, "has taken a fork");
    pthread_mutex_lock(&philo->state->forks[philo->right_fork]);
    print_status(philo->state, philo->id, "has taken a fork");
}

void eat(t_philosopher *philo)
{
    print_status(philo->state, philo->id, "is eating");
    philo->last_meal_time = get_time();
    smart_sleep(philo->state->time_to_eat, philo->state);
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->state->forks[philo->left_fork]);
    pthread_mutex_unlock(&philo->state->forks[philo->right_fork]);
}

void sleep_and_think(t_philosopher *philo)
{
    print_status(philo->state, philo->id, "is sleeping");
    smart_sleep(philo->state->time_to_sleep, philo->state);
    print_status(philo->state, philo->id, "is thinking");
}