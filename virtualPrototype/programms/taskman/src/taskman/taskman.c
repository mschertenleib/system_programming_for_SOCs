#include <assert.h>
#include <cache.h>
#include <defs.h>
#include <locks.h>
#include <taskman/taskman.h>

#include <implement_me.h>

/// @brief Maximum number of wait handlers.
#define TASKMAN_NUM_HANDLERS 32

/// @brief Maximum number of scheduled tasks.
#define TASKMAN_NUM_TASKS 128

/// @brief Maximum total stack size.
#define TASKMAN_STACK_SIZE (256 << 10)

#define TASKMAN_LOCK_ID 2

#define TASKMAN_LOCK()             \
    do {                           \
        get_lock(TASKMAN_LOCK_ID); \
    } while (0)

#define TASKMAN_RELEASE()              \
    do {                               \
        release_lock(TASKMAN_LOCK_ID); \
    } while (0)

__global static struct {
    /// @brief Wait handlers.
    struct taskman_handler* handlers[TASKMAN_NUM_HANDLERS];

    /// @brief Number of wait handlers;
    size_t handlers_count;

    /// @brief Stack area. Contains multiple independent stacks.
    uint8_t stack[TASKMAN_STACK_SIZE];

    /// @brief Stack offset (for the next allocation).
    size_t stack_offset;

    /// @brief Scheduled tasks.
    void* tasks[TASKMAN_NUM_TASKS];

    /// @brief Number of tasks scheduled.
    size_t tasks_count;

    /// @brief True if the task manager should stop.
    uint32_t should_stop;
} taskman;

/**
 * @brief Extra information attached to the coroutine used by the task manager.
 *
 */
struct task_data {
    struct {
        /// @brief Handler
        /// @note NULL if waiting on `coro_yield`.
        struct taskman_handler* handler;

        /// @brief Argument to the wait handler
        void* arg;
    } wait;

    /// @brief 1 if running, 0 otherwise.
    int running;
};

void taskman_glinit() {
    taskman.handlers_count = 0;
    taskman.stack_offset = 0;
    taskman.tasks_count = 0;
    taskman.should_stop = 0;
}

void* taskman_spawn(coro_fn_t coro_fn, void* arg, size_t stack_sz) {
    // (1) allocate stack space for the new task
    // (2) initialize the coroutine and struct task_data
    // (3) register the coroutine in the tasks array
    // use die_if_not() statements to handle error conditions (like no memory)

    TASKMAN_LOCK();

    // Allocate stack space
    uint8_t* task_stack = taskman.stack + taskman.stack_offset;
    taskman.stack_offset += stack_sz;
    die_if_not_f(taskman.stack_offset <= TASKMAN_STACK_SIZE, "Out of task manager stack space");

    // Initialize coroutine and task_data
    coro_init(task_stack, stack_sz, coro_fn, arg);
    struct task_data task_data = {
        .wait.handler = NULL, .wait.arg = NULL, .running = 0
    };
    *(struct task_data*)coro_data(task_stack) = task_data;

    // Register the coroutine in the tasks array
    die_if_not_f(taskman.tasks_count < TASKMAN_NUM_TASKS, "Maximum number of tasks (%d) exceeded", TASKMAN_NUM_TASKS);
    taskman.tasks[taskman.tasks_count] = task_stack;
    ++taskman.tasks_count;

    TASKMAN_RELEASE();

    return task_stack;
}

void taskman_loop() {
    // (a) Call the `loop` functions of all the wait handlers.
    // (b) Iterate over all the tasks, and resume them if.
    //        * The task is not complete.
    //        * it yielded using `taskman_yield`.
    //        * the waiting handler says it can be resumed.

    while (!taskman.should_stop) {

        TASKMAN_LOCK();
        // Call the loop function of all the wait handlers
        for (size_t i = 0; i < taskman.handlers_count; ++i) {
            taskman.handlers[i]->loop(taskman.handlers[i]);
        }
        size_t num_tasks = taskman.tasks_count;
        TASKMAN_RELEASE();

        // Iterate over all the tasks and resume them if necessary
        for (size_t i = 0; i < num_tasks; ++i) {
            void* task_stack = taskman.tasks[i];
            struct task_data* task_data = (struct task_data*)coro_data(task_stack);

            int complete = coro_completed(task_stack, NULL);
            int can_resume = 1;

            TASKMAN_LOCK();
            if (task_data->wait.handler != NULL) {
                can_resume = task_data->wait.handler->can_resume(task_data->wait.handler, task_stack, task_data->wait.arg);
            }

            if (!complete && !task_data->running && can_resume) {
                task_data->running = 1;
                coro_resume(task_stack);
            }
            TASKMAN_RELEASE();
        }
    }
}

void taskman_stop() {
    TASKMAN_LOCK();
    taskman.should_stop = 1;
    TASKMAN_RELEASE();
}

void taskman_register(struct taskman_handler* handler) {
    TASKMAN_LOCK();

    die_if_not(handler != NULL);
    die_if_not(taskman.handlers_count < TASKMAN_NUM_HANDLERS);

    taskman.handlers[taskman.handlers_count] = handler;
    taskman.handlers_count++;

    TASKMAN_RELEASE();
}

void taskman_wait(struct taskman_handler* handler, void* arg) {
    void* stack = coro_stack();
    struct task_data* task_data = coro_data(stack);

    // I suggest that you read `struct taskman_handler` definition.
    // Call handler->on_wait, see if there is a need to yield.
    // Update the wait field of the task_data.
    // Yield if necessary.

    TASKMAN_LOCK();

    int wait = 0;
    if (handler != NULL) {
        wait = handler->on_wait(handler, stack, arg);
        task_data->wait.handler = handler;
        task_data->wait.arg = arg;
    }

    if (wait == 0) {
        task_data->running = 0;
        coro_yield();
    }

    TASKMAN_RELEASE();
}

void taskman_yield() {
    taskman_wait(NULL, NULL);
}

void taskman_return(void* result) {
    coro_return(result);
}
