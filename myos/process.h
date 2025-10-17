#ifndef __PROCESS_H__
#define __PROCESS_H__
#include "kernel.h"
#include "size.h"
#include "allocator"
struct TSS64 {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed, aligned(16)));

struct GDTEntry64 {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed));

struct GDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

__attribute__((optimize("O0"), noinline))
void init_tss(uint64_t kernel_stack_phys, uint64_t ist1_phys);

class Process {
public:
    uint64_t cr3;
    uint64_t kernel_stack_phys;
    uint64_t user_stack_bottom;
    uint64_t user_stack_top;
    Process* next;
    uint64_t state; // always 1
    VirtPageAllocator* pallocator;
    uint8_t allocator_buffer[sizeof(VirtPageAllocator)];
    uint64_t code_va_base;
    uint64_t* kernel_stack;
    Process() = default;
    void init(uint64_t cs, uint64_t ss);
    void addCode(void* code_addr);
};
extern Process* now_process;
void init_process(Process* p);
void jmp_process();
#endif /*__PROCESS_H__*/