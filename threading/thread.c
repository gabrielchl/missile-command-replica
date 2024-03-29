#include "scheduler.h"
#include "thread.h"

#include <sys/mman.h>
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#define STACK_SIZE 1048576

unsigned int gen_thread_id() {
    static unsigned int id = 0;
    id++;
    return id;
}

void thread_wrapper(void (*fn)(void *param), void *param) {
    (*fn)(param);
    current_thread->state = STATE_END;
    schedule();
}

struct thread *thread_create(void (*fn)(void *param), void *param) {
    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    stack_t stack_struct = {
        .ss_sp = stack,
        .ss_size = STACK_SIZE
    };
    struct thread *thread = malloc(sizeof(*thread));
    *thread = (struct thread) {
        .id = gen_thread_id(),
        .state = STATE_RUNNING
    };
    getcontext(&thread->context);
    thread->context.uc_stack = stack_struct;
    makecontext(&thread->context, (void *)&thread_wrapper, 2, fn, param);
    return thread;
}
