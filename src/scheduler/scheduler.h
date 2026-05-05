#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "lib/other/types.h"

// task registers
typedef struct __attribute__((packed)){
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t eflags;
} scheduler_registers_t;

// task states
typedef enum{
    TASK_READY,
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_ZOMBIE
} task_state_t;

// TCB(Task Control Block)
typedef struct task{
    uint32_t pid_task;
    uint32_t esp;
    uint32_t wake_tick;
    task_state_t state_task;
    struct task* next_task;
    uint32_t* stack_base;
} task_t;

extern void switch_context(uint32_t* old_esp, uint32_t new_esp);
extern volatile task_t* current_task;

void init_schedule(void);

// main schedule loop
void loop_schedule(void);

// working with tasks
task_t* create_task(void(*func)(void));
void handler_task(void(*func)(void));
void exit_task(void);
void update_sleep_tasks(uint32_t current_timer_tisk);

#endif