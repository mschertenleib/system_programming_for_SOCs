#include <defs.h>
#include <taskman/semaphore.h>

#include <implement_me.h>

__global static struct taskman_handler semaphore_handler;

struct wait_data {
    // to be passed as an argument.
    // what kind of data do we need to put here
    // so that the semaphore works correctly?

    struct taskman_semaphore* semaphore;
    int direction; // 1: up, 0; down
};

static int impl(struct wait_data* wait_data) {
    // implement the semaphore logic here
    // do not forget to check the header file

    if (wait_data->direction == 0) {
        return wait_data->semaphore->count > 0;
    } else {
        return wait_data->semaphore->count < wait_data->semaphore->max;
    }
}

static int on_wait(struct taskman_handler* handler, void* stack, void* arg) {
    UNUSED(handler);
    UNUSED(stack);

    return impl((struct wait_data*)arg);
}

static int can_resume(struct taskman_handler* handler, void* stack, void* arg) {
    UNUSED(handler);
    UNUSED(stack);

    return impl((struct wait_data*)arg);
}

static void loop(struct taskman_handler* handler) {
    UNUSED(handler);
}

/* END SOLUTION */

void taskman_semaphore_glinit() {
    semaphore_handler.name = "semaphore";
    semaphore_handler.on_wait = &on_wait;
    semaphore_handler.can_resume = &can_resume;
    semaphore_handler.loop = &loop;

    taskman_register(&semaphore_handler);
}

void taskman_semaphore_init(
    struct taskman_semaphore* semaphore,
    uint32_t initial,
    uint32_t max
) {
    semaphore->count = initial;
    semaphore->max = max;
}

#include <printf.h>
void __no_optimize taskman_semaphore_down(struct taskman_semaphore* semaphore) {
    printf("Down %u %u\n", semaphore->count, semaphore->max);
    if (semaphore->count == 0) {
        struct wait_data wait_data = {
            .semaphore = semaphore, .direction = 0
        };
        taskman_wait(&semaphore_handler, (void*)&wait_data);
    }
    --semaphore->count;
    printf("Down done -> %u %u\n", semaphore->count, semaphore->max);
}

void __no_optimize taskman_semaphore_up(struct taskman_semaphore* semaphore) {
    printf("Up %u %u\n", semaphore->count, semaphore->max);
    if (semaphore->count >= semaphore->max) {
        struct wait_data wait_data = {
            .semaphore = semaphore, .direction = 1
        };
        taskman_wait(&semaphore_handler, (void*)&wait_data);
    }
    ++semaphore->count;
    printf("Up done -> %u %u\n", semaphore->count, semaphore->max);
}
