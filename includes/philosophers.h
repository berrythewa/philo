#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>

typedef struct s_philosopher
{
    int             id;
    int             left_fork;
    int             right_fork;
    int             meals_eaten;
    long long       last_meal_time;
    struct s_state  *state;
    pthread_t       thread;
} t_philosopher;


typedef struct s_state
{
    int             num_philosophers;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
    int             num_meals;
    int             someone_died;
    int             finished_eating;
    int             all_ate;
    long long       start_time;
    pthread_mutex_t *forks;
    pthread_mutex_t write_mutex;
    t_philosopher   *philosophers;
} t_state;



// Function prototypes
int         init_state(t_state *state, int argc, char **argv);
void        *philosopher_routine(void *arg);
void        take_forks(t_philosopher *philo);
void        eat(t_philosopher *philo);
void        sleep_and_think(t_philosopher *philo);
long long   get_time(void);
void        smart_sleep(long long duration, t_state *state);
void        print_status(t_state *state, int id, char *status);
int        ft_atoi(const char *str);
int         ft_atoi_lib(const char *str);
void        free_resources(t_state *state);

#endif