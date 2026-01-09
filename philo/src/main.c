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
    long long current_time;
    int is_dead = 0;

    pthread_mutex_lock(&state->write_mutex);  // Lock to ensure atomic check
    current_time = get_time();
    
    // Check if time since last meal exceeds time_to_die
    if (current_time - state->philosophers[i].last_meal_time > state->time_to_die)
    {
        if (!state->someone_died)
        {
            printf("%lld %d died\n", current_time - state->start_time, i + 1);
            state->someone_died = 1;
            is_dead = 1;
        }
    }
    pthread_mutex_unlock(&state->write_mutex);
    
    return is_dead;
}

static int check_meals(t_state *state)
{
    int i;
    int finished = 0;

    if (state->num_meals == -1)
        return (0);

    for (i = 0; i < state->num_philosophers; i++)
    {
        if (state->philosophers[i].meals_eaten >= state->num_meals)
            finished++;
    }

    if (finished == state->num_philosophers)
    {
        state->all_ate = 1;
        return (1);
    }
    return (0);
}

void *monitor_routine(void *arg)
{
    t_state *state = (t_state *)arg;
    int i;

    while (1)
    {
        // Check deaths
        for (i = 0; i < state->num_philosophers; i++)
        {
            if (check_death(state, i))
                return (NULL);
        }

        // Check if all philosophers have eaten enough
        if (check_meals(state))
            return (NULL);

        usleep(500);  // Small sleep to prevent CPU hogging
    }
    return (NULL);
}


int main(int argc, char **argv)
{
    t_state state;
    pthread_t monitor_thread;

    if (argc != 5 && argc != 6)
    {
        printf("Error: Invalid number of arguments\nUsage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
        return (1);
    }

    if (init_state(&state, argc, argv) != 0)
    {
        printf("Error: Failed to initialize state\n");
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