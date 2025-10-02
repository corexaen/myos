#ifndef __LOG_H__
#define __LOG_H__
void uart_init();

__attribute__((no_caller_saved_registers))
int uart_is_transmit_empty();

__attribute__((no_caller_saved_registers))
void uart_putc(char c);

__attribute__((no_caller_saved_registers))
void uart_print(const char* s);
#endif // __LOG_H__