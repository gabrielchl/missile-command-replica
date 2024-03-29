#include "list.h"
#include "thread.h"
#include "scheduler.h"
#include "sleeper.h"

#include <pthread.h>
#include <time.h>
#include <unistd.h>

static struct list_item *sleeping_threads;

int sleep_sort(void *a, void *b) {
    struct timespec a_time = ((struct sleeping_thread *)a)->wakeup;
    struct timespec b_time = ((struct sleeping_thread *)b)->wakeup;
    if (a_time.tv_sec == b_time.tv_sec)
        return (long)a_time.tv_nsec - b_time.tv_nsec;
    return (long)a_time.tv_sec - b_time.tv_sec;
}

void sleep_add(unsigned int sec, unsigned int nsec) {
    struct timespec wakeup;
    clock_gettime(CLOCK_REALTIME, &wakeup);
    wakeup.tv_sec += sec;
    wakeup.tv_nsec += nsec;
    if (wakeup.tv_nsec >= 1000000000) {
        wakeup.tv_sec += 1;
        wakeup.tv_nsec -= 1000000000;
    }
    struct sleeping_thread *sleeping_thread = malloc(sizeof(*sleeping_thread));
    *sleeping_thread = (struct sleeping_thread){
        .thread = current_thread,
        .wakeup = wakeup
    };
    push_item_order(&sleeping_threads, sleeping_thread, &sleep_sort);

    current_thread->state = STATE_IDLE;
    schedule();
    current_thread->state = STATE_RUNNING;
}

void sleep_till_next() {
    if (!sleeping_threads) {
        pause();
    } else {
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        struct timespec wakeup_time = ((struct sleeping_thread *)sleeping_threads->content)->wakeup;
        if (wakeup_time.tv_sec > spec.tv_sec || (wakeup_time.tv_sec == spec.tv_sec && wakeup_time.tv_nsec > spec.tv_nsec)) {
            if (wakeup_time.tv_nsec < spec.tv_nsec) {
                wakeup_time.tv_sec -= 1;
                wakeup_time.tv_nsec += 1000000000;
            }
            struct timespec sleep_length = {
                .tv_sec = wakeup_time.tv_sec - spec.tv_sec,
                .tv_nsec = wakeup_time.tv_nsec - spec.tv_nsec
            };
            pthread_mutex_unlock(&in_sleep);
            nanosleep(&sleep_length, NULL);
            pthread_mutex_lock(&in_sleep);
            clock_gettime(CLOCK_REALTIME, &spec);
        }

        while (
            sleeping_threads &&
            ((wakeup_time = ((struct sleeping_thread *)sleeping_threads->content)->wakeup),
            (wakeup_time.tv_sec < spec.tv_sec || (wakeup_time.tv_sec == spec.tv_sec && wakeup_time.tv_nsec <= spec.tv_nsec)))
        )  {
            struct sleeping_thread *item = pop_item_front(&sleeping_threads);
            sched_wakeup(item->thread);
            free(item);
        }
    }
}
