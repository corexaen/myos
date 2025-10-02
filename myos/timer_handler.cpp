#include "timer_handler.h"
#include "kernel.h"
#include "lapic.h"
#include "log.h"
#include "util.h"
#include "memory.h"

char uart_buf[1000];
extern "C" __attribute__((noinline, no_caller_saved_registers)) uint64_t* c_timer_handler(context_t* frame) {
    current = next;
    next = frame;
    uart_print("timer\n");
    memset(uart_buf, 0, sizeof(uart_buf));
    bytes_to_hex_string((char*)current, 8 * 20, uart_buf);
    uart_print(uart_buf);

    lapic_eoi();
    uint64_t* ret = (uint64_t*)current;
    asm volatile("" : "+a"(ret));
    return (uint64_t*)current;
}