#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include "kernel.h"
#include "idt.h"
extern "C" __attribute__((noinline)) void syscall_handler(context_t* frame);
#endif /* __SYSCALL_H__ */