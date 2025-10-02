extern "C" void main();
#include "kernel.h"
extern "C" __attribute__((naked, section(".entry"))) void _start() {
    __asm__ __volatile__(
        //"hlt;"
        "jmp main;"
        "hlt;"
    );
}