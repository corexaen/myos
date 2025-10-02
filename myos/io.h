#ifndef __IO_H__
#define __IO_H__
#include "size.h"

__attribute__((no_caller_saved_registers))
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("out dx, al" :: "a"(val), "d"(port));
}

__attribute__((no_caller_saved_registers))
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("in al, dx" : "=a"(ret) : "d"(port));
    return ret;
}

#endif // __IO_H__