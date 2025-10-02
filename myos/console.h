#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include "kernel.h"

//todo - BootInfo��� frame������ ������ ����ü�� ����� �ѱ��
__attribute__((no_caller_saved_registers))
void putc(BootInfo* f, int x, int y, char text, uint32_t color, int scale);
#endif // __CONSOLE_H__