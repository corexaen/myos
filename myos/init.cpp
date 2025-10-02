#include "kernel.h"
#include "memory.h"
#include "allocator"
#include "util.h"
#include "console.h"
#include "log.h"
#include "idt.h"
#include "ioapic.h"
#include "lapic.h"
#include "handler.h"
#include "timer_handler.h"

// 최종 APIC 초기화
void init_apic() {
    disable_pic();               // 1. PIC 끄고
    init_ioapic_base();
    init_lapic_base();
    enable_apic();               // 2. Local APIC 켜고
    setup_lapic_timer(32);
    ioapic_set_redirection(1, 0x21, 0);
    ioapic_set_redirection(12, 0x2C, 0);
}
int i1 = 0;
int i2 = 0;
//task test
extern "C" __attribute__((noreturn, noinline)) void task1_func() {
    int a = 0;
    int b;
    while (1) {
        for (i1 = 0; i1 < bootinfo->framebufferPitch * bootinfo->framebufferHeight / 2; i1++) {
            *((uint32_t*)(bootinfo->framebufferAddr) + i1) = a << 16;
        }
        a = (a + 1) % 0x100;
        b++;
    }
}
extern "C" __attribute__((noreturn, noinline)) void task2_func() {
    int a = 0;
    int c;
    while (1) {
        for (i2 = bootinfo->framebufferPitch * bootinfo->framebufferHeight / 2; i2 < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i2++) {
            *((uint32_t*)(bootinfo->framebufferAddr) + i2) = a;
        }
        a = (a + 1) % 0x100;
        c++;
    }
}

void init_tasks() {
	uint64_t* task1_rsp = (uint64_t*)phy_page_allocator->alloc_phy_page();
	virt_page_allocator->alloc_virt_page((uint64_t)task1_rsp, (uint64_t)task1_rsp, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::G);
    task1_rsp = (uint64_t*)((uint64_t)task1_rsp + PageSize);
    
    *(--task1_rsp) = 0; //16정렬용
    *(--task1_rsp) = 0x10; //ss
    *(--task1_rsp) = 0;
	*(--task1_rsp) = 0x202; // rflags
	*(--task1_rsp) = 0x08;  // cs
	*(--task1_rsp) = (uint64_t)task1_func; // rip
    task1_rsp[3] = (uint64_t)task1_rsp;
	uint64_t* task2_rsp = (uint64_t*)phy_page_allocator->alloc_phy_page();
	virt_page_allocator->alloc_virt_page((uint64_t)task2_rsp, (uint64_t)task2_rsp, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::G);
	task2_rsp = (uint64_t*)((uint64_t)task2_rsp + PageSize);
	*(--task2_rsp) = 0; //16정렬용
	*(--task2_rsp) = 0x10;
	*(--task2_rsp) = 0;
	*(--task2_rsp) = 0x202; // rflags
	*(--task2_rsp) = 0x08;  // cs
	*(--task2_rsp) = (uint64_t)task2_func; // rip
    
    for(int i=0;i<15 ;i++) {
        *(--task2_rsp) = 0;
	}
	task2_rsp[18] = (uint64_t)(task2_rsp + 15); // rsp
    current = (context_t*)task1_rsp;
    next = (context_t*)task2_rsp;
}

static uint8_t console[100 * 40] = { 0, };



void init_interrupts() {
    asm volatile ("cli");
    uart_init();
	init_allocators(bootinfo->physbm, bootinfo->physbm_size);
    init_tasks();
    init_apic();
    for (int i = 0; i < IDT_SIZE; i++) {
        set_idt_gate(i, (uint64_t)none_handler, 0x08, 0x8E);
    }
	set_idt_gate(13, (uint64_t)general_protection_fault_handler, 0x08, 0x8E);
	set_idt_gate(14, (uint64_t)page_fault_handler, 0x08, 0x8E);
	set_idt_gate(12, (uint64_t)stack_segment_fault_handler, 0x08, 0x8E);

    set_idt_gate(32, (uint64_t)timer_handler, 0x08, 0x8E);
    set_idt_gate(33, (uint64_t)keyboard_handler, 0x08, 0x8E);
    //set_idt_gate(0x2C, (uint64_t)dummy_mouse_handler, 0x08, 0x8E);
    load_idt();
	asm volatile ("sti");
}
extern "C" uint64_t _rsp = 0xFF;
int x, y, i;
char raw_stack[1000];
extern char uart_buf[1000];
extern "C" __attribute__((force_align_arg_pointer, noinline)) void main() {
	//__asm__ __volatile__("hlt");
    init_interrupts();
    
    __asm__ __volatile__(
        
        //"push rdi\n\t"
        //"mov rdi, rsp\n\t"
        //"mov rsp, 0x600000\n\t"
        "mov rsp, %[in]\n\t"
        /*
        "pop r15\n\t"
        "pop r14\n\t"
        "pop r13\n\t"
        "pop r12\n\t"
        "pop r11\n\t"
        "pop r10\n\t"
        "pop r9\n\t"
        "pop r8\n\t"
        "pop rbp\n\t"
        "pop rdi\n\t"
        "pop rsi\n\t"
        "pop rdx\n\t"
        "pop rcx\n\t"
        "pop rbx\n\t"
        "pop rax\n\t"
        */
        /*
        "and rsp, -16\n\t"
        "sub rsp, 8\n\t"
        "pushfq\n\t"
        "pop rax\n\t"
        "or rax, 0x200\n\t"
        "push rax\n\t"
        "mov rax, 0x08\n\t"
        "push rax\n\t"
        "push %[entry]\n\t"
        */
        //"iretq\n\t"
		"mov %[out], rsp\n\t"
        /*
        "call .tetestst\n\t"
        ".tetestst:\n\t"
        "pop rax\n\t"
        "mov %[out], rax\n\t"
        */
		//"sub rsp, 8\n\t"
        //"mov rsp, rdi\n\t"
        //"pop rdi\n\t"
        
        :
    [out] "=m"(_rsp)              // 전역 변수 memory output
        : [in] "m"(current),
        [entry] "r"(task1_func)
        : "rax", "memory"
        );
    memcpy(raw_stack, (void*)&_rsp, 8);
    bytes_to_hex_string(raw_stack, 8, (char*)console);
    memcpy(raw_stack, (void*)_rsp, 24);
	bytes_to_hex_string(raw_stack, 24, (char*)console + 100);
    //console[3] = 'B';
    //__asm__ __volatile__("hlt");
    while (1) {
		memcpy(console, uart_buf, 3 * 8 * 20);
        bytes_to_hex_string((char*)next, 8 * 20, (char*)console + 7 * 96);
        for (i = 0; i < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i++) {
            *((uint32_t*)(bootinfo->framebufferAddr) + i) = 0xFFFFFF;
        }
        for (y = 0; y < 40; y++) {
            for (x = 0; x < 96; x++) {
                putc(bootinfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 96 + x], 0, 1);
            }
        }
		__asm__ __volatile__("hlt");
    }
}