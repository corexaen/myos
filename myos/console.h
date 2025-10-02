#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include "kernel.h"

//todo - BootInfo대신 frame정보를 저장할 구조체를 만들어 넘기기
__attribute__((no_caller_saved_registers))
void putc(BootInfo* f, int x, int y, char text, uint32_t color, int scale);
#endif // __CONSOLE_H__