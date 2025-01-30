#include "../includes/philosophers.h"

long long get_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void precise_sleep(long long microseconds)
{
#ifdef __APPLE__
    usleep(microseconds);
#else
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
#endif
}

int smart_sleep(long long duration, t_state *state)
{
    long long start;
    long long now;
    long long elapsed;

    start = get_time();
    while (!(state->someone_died))
    {
        now = get_time();
        elapsed = now - start;
        
        // Handle potential overflow for large durations
        if (elapsed < 0 || elapsed >= duration)
            break;
            
        // Use smaller sleep intervals for large durations
        long long remaining = duration - elapsed;
        long long sleep_time = (remaining > 1000) ? 1000 : remaining;
        precise_sleep(sleep_time * 1000);  // Convert to microseconds
    }
    return (0);
}