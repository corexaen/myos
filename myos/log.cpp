#include "log.h"
#include "io.h"
#define COM1 0x3F8
void uart_init() {
    outb(COM1 + 1, 0x00);    // Disable interrupts
    outb(COM1 + 3, 0x80);    // Enable DLAB
    outb(COM1 + 0, 0x03);    // Baud divisor (38400 baud)
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop
    outb(COM1 + 2, 0xC7);    // Enable FIFO
    outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int uart_is_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

void uart_putc(char c) {
    while (!uart_is_transmit_empty());
    outb(COM1, c);
}

void uart_print(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}