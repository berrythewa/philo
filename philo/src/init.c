#include "../includes/philosophers.h"

static int validate_args(int argc, char **argv)
{
    int i;
    long temp;

    for (i = 1; i < argc; i++)
    {
        temp = ft_atoi_lib(argv[i]);
        if (temp <= 0 || temp > INT_MAX)
        {
            printf("Error: Invalid argument '%s'. \
                All arguments must be non-negative integers and greater than 0.\n",
                argv[i]
            );
            return (1);
        }
        
    }
    return (0);
}

static int init_mutexes(t_state *state)
{
    int i;

    state->forks = malloc(sizeof(pthread_mutex_t) * state->num_philosophers);
    if (!state->forks)
        return (1);
    
    for (i = 0; i < state->num_philosophers; i++)
    {
        if (pthread_mutex_init(&state->forks[i], NULL) != 0)
        {
            // Destroy already initialized mutexes
            while (--i >= 0)
                pthread_mutex_destroy(&state->forks[i]);
            free(state->forks);
            return (1);
        }
    }
    
    if (pthread_mutex_init(&state->write_mutex, NULL) != 0)
    {
        // Destroy fork mutexes
        for (i = 0; i < state->num_philosophers; i++)
            pthread_mutex_destroy(&state->forks[i]);
        free(state->forks);
        return (1);
    }
    
    return (0);
}

static int init_philosophers(t_state *state)
{
    int i;

    state->philosophers = malloc(sizeof(t_philosopher) * state->num_philosophers);
    if (!state->philosophers)
        return (1);
    
    for (i = 0; i < state->num_philosophers; i++)
    {
        state->philosophers[i].id = i + 1;
        state->philosophers[i].left_fork = i;
        state->philosophers[i].right_fork = (i + 1) % state->num_philosophers;
        state->philosophers[i].meals_eaten = 0;
        state->philosophers[i].last_meal_time = state->start_time;
        state->philosophers[i].state = state;
    }
    
    return (0);
}

int init_state(t_state *state, int argc, char **argv)
{
    if (validate_args(argc, argv))
        return (1);

    state->num_philosophers = ft_atoi(argv[1]);
    state->time_to_die = ft_atoi(argv[2]);
    state->time_to_eat = ft_atoi(argv[3]);
    state->time_to_sleep = ft_atoi(argv[4]);
    state->num_meals = (argc == 6) ? ft_atoi(argv[5]) : -1;
    state->someone_died = 0;
    state->finished_eating = 0;
    state->all_ate = 0;
    state->start_time = get_time();

    if (state->num_philosophers == 0 || state->time_to_die == 0 || 
        state->time_to_eat == 0 || state->time_to_sleep == 0)
    {
        printf("Error: All time values must be greater than 0.\n");
        return (1);
    }

    if (init_mutexes(state) != 0 || init_philosophers(state) != 0)
    {
        free_resources(state);
        return (1);
    }

    return (0);
}