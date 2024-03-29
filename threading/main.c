#include "list.h"
#include "thread.h"
#include "listener.h"
#include "scheduler.h"
#include "sleeper.h"

#include <ucontext.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

void print_1(void *param) { // demo function
    while(1) {
        printf("1\n");
        sleep_add(1, 0);
    }
}

void print_2(void *param) { // demo function
    while(1) {
        printf("2\n");
        sleep_add(2, 0);
    }
}

void print_3(void *param) { // demo function
    while(1) {
        printf("3\n");
        sleep_add(3, 0);
    }
}

void signal_dummy() {
    //printf("Signal triggered\n");
    return;
}

int main() {
    pthread_mutex_init(&in_sleep, NULL);
    sched_init();
    signal(SIGUSR1, signal_dummy);
    input_init();

    //struct thread *thread_1 = thread_create(&print_1, NULL);
    //sched_wakeup(thread_1);

    //struct thread *thread_2 = thread_create(&print_2, NULL);
    //sched_wakeup(thread_2);

    struct thread *thread_3 = thread_create(&print_3, NULL);
    sched_wakeup(thread_3);

    while(1) {
        schedule();
        sleep_wait();
        // something like "errno != EINTR" here
    }

    printf("END!!!\n");

    return 0;
}
