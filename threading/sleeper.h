#ifndef SLEEPER_H
#define SLEEPER_H

#include "thread.h"
#include "scheduler.h"

#include <time.h>

struct sleeping_thread {
    struct thread *thread;
    struct timespec wakeup;
};

extern pthread_mutex_t in_sleep;

/**
* Adds sleeping_thread to the sleeping_threads list, ordered by wakeup time.
*/
void sleep_add(unsigned int sec, unsigned int nsec);

/**
* Checks if the first sleeping_thread should wake up, if so, calls sched_wakeup,
* also removing the sleeping_thread from the sleeping_threads list.
*/
void sleep_till_next();

void swapper();

#endif
