#include "timer_handler.h"
#include "kernel.h"
#include "lapic.h"
#include "log.h"
#include "util.h"
#include "memory.h"
#include "process.h"
char uart_buf[1000];
extern "C" __attribute__((noinline)) uint64_t* c_timer_handler(context_t* frame) {
    now_process->kernel_stack = (uint64_t*)frame;
    now_process = now_process->next;
    lapic_eoi();
    jmp_process();
    //uint64_t* ret = (uint64_t*)current;
    //asm volatile("" : "+a"(ret));
    return (uint64_t*)0;
}