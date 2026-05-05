#include "timer.h"

#include "arch/i386/io.h"
#include "src/scheduler/scheduler.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQUENCY 1193182
#define PIT_INIT_COMMAND 0x36
#define MASK_LOW_8_BITS 0xFF
#define OFFSET_HIGH_8_BITS 8
#define MIN_FREQ 10
#define MAX_FREQ 1000
#define MILLISECONDS_PER_SECOND 1000

volatile uint32_t timer_tick = 0;
volatile uint32_t timer_frequency = DEFAULT_TIMER_FREQUENCY;

// working with a timer
void init_timer(uint32_t frequency){
    timer_frequency = frequency;
    uint32_t divisor =  PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, PIT_INIT_COMMAND);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & MASK_LOW_8_BITS));
    outb(PIT_CHANNEL0, (uint8_t)(divisor >> OFFSET_HIGH_8_BITS) & MASK_LOW_8_BITS);
}

void handler_timer(void){
    timer_tick++;
    update_sleep_tasks(timer_tick);
    outb(PIC1_COMMAND_PORT, PIC_EOI);
    loop_schedule();
}

// working with the timer frequency
void set_timer_frequency(uint32_t time){
    if(time < MIN_FREQ || time > MAX_FREQ){return;}
    timer_frequency = time;
    init_timer(time);
}

uint32_t get_timer_frequency(void){
    return timer_frequency;
}

// working with time
void sleep(uint32_t ms){
    uint32_t timer_tick_to_wait = (ms * timer_frequency) / MILLISECONDS_PER_SECOND;

    current_task->wake_tick = timer_tick + timer_tick_to_wait;
    current_task->state_task = TASK_SLEEPING;

    loop_schedule();
}