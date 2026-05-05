#include "scheduler.h"

#include "lib/data_structures/ringbuffer.h"
#include "src/memory/kmalloc.h"

#define SIZE_SCHEDULE_STACK 4096

volatile task_t* current_task = NULL;
volatile uint32_t pid_count = 1;

void init_schedule(void){
    current_task = (task_t*)kcalloc(1, sizeof(task_t));
    if(NULL == current_task){return;}

    uint32_t* stack = (uint32_t*)kmalloc(SIZE_SCHEDULE_STACK);
    if(NULL == stack){kfree((void*)current_task); current_task = NULL; return;}
    
    uint32_t* esp = (uint32_t*)(((uint32_t)stack + SIZE_SCHEDULE_STACK) & ~0xF);

    current_task->pid_task = 0;
    current_task->next_task = (task_t*)current_task;
    current_task->state_task = TASK_RUNNING;
    current_task->stack_base = stack;
    current_task->esp = (uint32_t)esp;

    __asm__ volatile("mov %%esp, %0" : "=r"(current_task->esp));
}

// main schedule loop
void loop_schedule(void){
    if(NULL == current_task){return;}

    task_t* prev = (task_t*)current_task;
    task_t* next = (task_t*)current_task->next_task;

    while(next->state_task != TASK_READY && next->state_task != TASK_RUNNING){
        next = next->next_task;
        if(next == prev){
            if(TASK_SLEEPING == prev->state_task || TASK_ZOMBIE == prev->state_task){
                __asm__ volatile("sti; hlt");
                return;
            }
            return;
        }   
    }

    if(next == prev){return;}

    if(TASK_RUNNING == prev->state_task){
        prev->state_task = TASK_READY;
    }
    next->state_task = TASK_RUNNING;
    current_task = next;

    switch_context(&(prev->esp), next->esp);
}

// working with tasks
task_t* create_task(void(*func)(void)){
    if(NULL == func){return NULL;}

    task_t* new_task = (task_t*)kcalloc(1, sizeof(task_t));
    if(NULL == new_task){return NULL;}
    
    uint32_t* stack = (uint32_t*)kmalloc(SIZE_SCHEDULE_STACK);
    if(NULL == stack){kfree(new_task); return NULL;}

    uint32_t* esp = (uint32_t*)(((uint32_t)stack + SIZE_SCHEDULE_STACK) & ~0xF);

    *(--esp) = (uint32_t)func;
    *(--esp) = 0;
    *(--esp) = (uint32_t)handler_task; // EIP
    *(--esp) = 0; // EBP
    *(--esp) = 0; // EBX
    *(--esp) = 0; // ESI
    *(--esp) = 0; // EDI
    
    new_task->pid_task = pid_count++;
    new_task->esp = (uint32_t)esp;
    new_task->state_task = TASK_READY;
    new_task->stack_base = stack;

    new_task->next_task = current_task->next_task;
    current_task->next_task = new_task;

    return new_task;
}

void handler_task(void(*func)(void)){
    __asm__ volatile("sti");
    func();
    exit_task();
}

void exit_task(void){
    __asm__ volatile("cli");

    current_task->state_task = TASK_ZOMBIE;
    loop_schedule();

    while(true){__asm__ volatile("hlt");}
}

void update_sleep_tasks(uint32_t current_timer_tisk){
    if(NULL == current_task){return;}

    task_t* start = (task_t*)current_task;
    task_t* current = start;

    do{
        if(NULL == current){break;}
        if(TASK_SLEEPING == current->state_task && current_timer_tisk >= current->wake_tick){
            current->state_task = TASK_READY;
        }
        current = current->next_task;
    } while(current != start && NULL != current);
}