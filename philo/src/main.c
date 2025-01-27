#include "../includes/philosophers.h"

static void create_threads(t_state *state)
{
    int i;

    for (i = 0; i < state->num_philosophers; i++)
    {
        if (pthread_create(&state->philosophers[i].thread, NULL, philosopher_routine, &state->philosophers[i]) != 0)
        {
            printf("Error creating thread for philosopher %d\n", i + 1);
            return;
        }
        printf("Created thread for philosopher %d\n", i + 1);
    }
}

static void join_threads(t_state *state)
{
    int i;

    for (i = 0; i < state->num_philosophers; i++)
    {
        if (pthread_join(state->philosophers[i].thread, NULL) != 0)
        {
            printf("Error joining thread for philosopher %d\n", i + 1);
            return;
        }
        printf("Joined thread for philosopher %d\n", i + 1);
    }
}

static int check_death(t_state *state, int i)
{
    long long current_time = get_time();
    if (current_time - state->philosophers[i].last_meal_time > state->time_to_die)
    {
        pthread_mutex_lock(&state->write_mutex);
        if (!state->someone_died)
        {
            printf("%lld %d died\n", current_time - state->start_time, i + 1);
            state->someone_died = 1;
        }
        pthread_mutex_unlock(&state->write_mutex);
        return (1);
    }
    return (0);
}

void *monitor_routine(void *arg)
{
    t_state *state = (t_state *)arg;
    int i;

    while (!state->someone_died && !state->all_ate)
    {
        for (i = 0; i < state->num_philosophers; i++)
        {
            if (check_death(state, i))
                return (NULL);
        }
        usleep(1000);  // Sleep for 1ms to reduce CPU usage
    }
    return (NULL);
}


int main(int argc, char **argv)
{
    t_state state;
    pthread_t monitor_thread;

    if (argc != 5 && argc != 6)
    {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
        return (1);
    }

    if (init_state(&state, argc, argv) != 0)
    {
        if (ft_atoi_lib(argv[1]) == 0 || ft_atoi_lib(argv[2]) == 0)
        {
            // This is the "instant death" case
            // message already printed in init_state
            return (0);
        }
        printf("Error initializing state\n");
        return (1);
    }

    create_threads(&state);
    
    if (pthread_create(&monitor_thread, NULL, monitor_routine, &state) != 0)
    {
        printf("Error creating monitor thread\n");
        return (1);
    }

    pthread_join(monitor_thread, NULL);  // Wait for monitor to detect death or all philosophers to finish eating

    state.someone_died = 1;  // Signal all threads to stop
    join_threads(&state);

    free_resources(&state);

    return (0);
}