#include "scheduler.h"

#include "arch/i386/tss.h"
#include "lib/data_structures/ringbuffer.h"
#include "src/memory/kmalloc.h"

#define SIZE_SCHEDULE_STACK 4096
#define SIZE_USER_STACK 4096
#define ALIGNMENT_16_BYTE 0xF

volatile task_t* current_task = NULL;
volatile uint32_t pid_count = 1;
extern uint32_t stack_top;
extern void jump_user_ring(void* entry_point, void* user_stack);
/////////////////
#include "src/terminal/terminal.h"
#include "lib/print/print.h"
/////////////////
void init_schedule(void){
    current_task = (task_t*)kcalloc(1, sizeof(task_t));
    if(NULL == current_task){return;}

    __asm__ volatile("mov %%esp, %0" : "=r"(current_task->kernel_esp));
    
    current_task->stack_base = &stack_top;

    current_task->pid_task = 0;
    current_task->next_task = (task_t*)current_task;
    current_task->state_task = TASK_RUNNING;
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
    
    tss_set_stack((uint32_t)next->stack_base + SIZE_SCHEDULE_STACK);
    
    switch_context(&(prev->kernel_esp), next->kernel_esp);
}

// working with tasks
task_t* create_task(void(*func)(void)){
    if(NULL == func){return NULL;}

    task_t* new_task = (task_t*)kcalloc(1, sizeof(task_t));
    if(NULL == new_task){return NULL;}

    uint32_t* kstack = (uint32_t*)kcalloc(1, SIZE_SCHEDULE_STACK);
    if(NULL == kstack){print_str("testadsdasd\n"); return NULL;}    

    uint32_t* ustack = (uint32_t*)kcalloc(1, SIZE_USER_STACK);
    if(NULL == ustack){kfree(kstack); kstack = NULL; return NULL;}

    uint32_t* k_esp = (uint32_t*)(((uint32_t)kstack + SIZE_SCHEDULE_STACK) & ~ALIGNMENT_16_BYTE);
    uint32_t* u_esp = (uint32_t*)(((uint32_t)ustack + SIZE_USER_STACK) & ~ALIGNMENT_16_BYTE);
    
    *(--k_esp) = (uint32_t)func;
    *(--k_esp) = (uint32_t)handler_task;
    *(--k_esp) = 0; // ebp
    *(--k_esp) = 0; // edi
    *(--k_esp) = 0; // esi
    *(--k_esp) = 0; // ebx

    new_task->pid_task = pid_count++;
    new_task->state_task = TASK_READY;
    new_task->stack_base = kstack;
    new_task->kernel_esp = (uint32_t)k_esp;
    new_task->user_esp = (uint32_t)u_esp;
    new_task->entry_point = func;
    new_task->next_task = current_task->next_task;
    current_task->next_task = new_task;
    
    return new_task;
}

void handler_task(void(*func)(void)){
    __asm__ volatile("sti");

    jump_user_ring(current_task->entry_point, (uint32_t*)current_task->user_esp);
    
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