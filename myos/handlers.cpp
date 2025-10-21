#include "kernel.h"
#include "handler.h"
#include "io.h"
#include "util.h"
#include "console.h"
#include "log.h"
#include "lapic.h"
#include "memory.h"

static uint8_t console[100 * 40] = { 0, }; // ������ �ܼ� ����

//todo - ���� �κа� �����Ӻκ� �и� �� ���ο� ���Ϸ� ����
__attribute__((interrupt))
void keyboard_handler(interrupt_frame_t* frame) {
    uint8_t scancode = inb(0x60);
    for (int i = 0; i < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i++) {
        uint8_t Red = 0;
        uint8_t Green = simple_rand() % 256;
        uint8_t Blue = 255;

        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(bootinfo->framebufferAddr) + i) = PixelColor;
    }

    /*
    if (console[0] == 0) {
        char raw_frame[24];
        memcpy(raw_frame, frame, 24);
        bytes_to_hex_string(raw_frame, 24, (char*)console);
    }
    */
    lapic_eoi();
}
__attribute__((interrupt))
void dummy_mouse_handler(interrupt_frame_t* frame) {
    inb(0x60);
    for (int i = 0; i < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i++) {
        uint8_t Red = 255;
        uint8_t Green = 255;
        uint8_t Blue = 0;

        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(bootinfo->framebufferAddr) + i) = PixelColor;
    }
    lapic_eoi();
}
__attribute__((naked))
void timer_handler() {
    asm volatile(
        // ���ͷ�Ʈ ���Խ� Ʈ�� �������� �̹� ���ÿ� ���� (rip, cs, rflags, [rsp, ss])
        // �߰������� ����/�Ϲ� �������͵� ����
        "push rax\n\t"
        "push rbx\n\t"
        "push rcx\n\t"
        "push rdx\n\t"
        "push rsi\n\t"
        "push rdi\n\t"
        "push rbp\n\t"
        "push r8\n\t"
        "push r9\n\t"
        "push r10\n\t"
        "push r11\n\t"
        "push r12\n\t"
        "push r13\n\t"
        "push r14\n\t"
        "push r15\n\t"
        "mov rax, ds\n\t"
        "push rax\n\t"
        "mov rax, es\n\t"
        "push rax\n\t"
        "mov rax, fs\n\t"
        "push rax\n\t"
        "mov rax, gs\n\t"
        "push rax\n\t"
        // ���� ���� ����:
        // [r15][r14]...[rax][RIP][CS][RFLAGS]([RSP][SS]) <- rsp

        // ���� ������ �ּҸ� ù ��° ����(rdi)�� �ѱ���!
        "mov rdi, rsp\n\t"
        // C�� ����! (C���� �����ٸ�/���ؽ�Ʈ����Ī/���� �Ǵ�)
        "call c_timer_handler\n\t"
        "mov rsp, rax\n\t"  // C �Լ��� ��ȯ�� rsp�� ����
        // C �Լ��� ������ context�� trapframe/�������͸� ���ÿ� push�ص�(���� ���μ���)
        // ���⼭ ���͸� �ϸ� ��

        // pop ������� pop
        "pop rax\n\t"
        "mov gs, ax\n\t"
        "pop rax\n\t"
        "mov fs, ax\n\t"
        "pop rax\n\t"
        "mov es, ax\n\t"
        "pop rax\n\t"
        "mov ds, ax\n\t"
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

        // ���� Ʈ�� �����Ӹ� ���ÿ� ���� ����
        // iretq�� ���� (RIP, CS, RFLAGS, [RSP, SS])�� �ڵ����� pop
        "iretq\n\t"
        );
}
__attribute__((interrupt))
void none_handler(interrupt_frame_t* frame) {
    __asm__ __volatile__("hlt");
    lapic_eoi();
}
__attribute__((interrupt))
void general_protection_fault_handler(interrupt_frame_t* frame, uint64_t error_code) {
    for (int i = 0; i < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i++) {
        *((uint32_t*)(bootinfo->framebufferAddr) + i) = 0xFFFFFF;
    }
    char raw_stack[8];
    memcpy(raw_stack, (void*)&error_code, 8);
    bytes_to_hex_string(raw_stack, sizeof(raw_stack), (char*)console);
    console[3 * 9] = 'G';
    console[3 * 9 + 1] = 'F';
    while (1) {
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 100; x++) {
                putc(bootinfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 100 + x], 0, 1);
            }
        }
    }
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt))
void stack_segment_fault_handler(interrupt_frame_t* frame, uint64_t error_code) {
    char raw_stack[8];
    memcpy(raw_stack, (void*)&error_code, 8);
    bytes_to_hex_string(raw_stack, sizeof(raw_stack), (char*)console);
    console[3 * 9] = 'S';
    console[3 * 9 + 1] = 'S';
    for (int i = 0; i < bootinfo->framebufferPitch * bootinfo->framebufferHeight; i++) {
        *((uint32_t*)(bootinfo->framebufferAddr) + i) = 0xFFFFFF;
    }
    while (1) {
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 100; x++) {
                putc(bootinfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 100 + x], 0, 1);
            }
        }
    }
    __asm__ __volatile__("hlt");
}
__attribute__((naked))
void syscall_idthandler() {
    asm volatile(
        // ���ͷ�Ʈ ���Խ� Ʈ�� �������� �̹� ���ÿ� ���� (rip, cs, rflags, [rsp, ss])
        // �߰������� ����/�Ϲ� �������͵� ����
        "push rax\n\t"
        "push rbx\n\t"
        "push rcx\n\t"
        "push rdx\n\t"
        "push rsi\n\t"
        "push rdi\n\t"
        "push rbp\n\t"
        "push r8\n\t"
        "push r9\n\t"
        "push r10\n\t"
        "push r11\n\t"
        "push r12\n\t"
        "push r13\n\t"
        "push r14\n\t"
        "push r15\n\t"
        "mov rax, ds\n\t"
        "push rax\n\t"
        "mov rax, es\n\t"
        "push rax\n\t"
        "mov rax, fs\n\t"
        "push rax\n\t"
        "mov rax, gs\n\t"
        "push rax\n\t"

        "mov rdi, rsp\n\t"
        "call syscall_handler\n\t"
        "pop rax\n\t"
        "mov gs, ax\n\t"
        "pop rax\n\t"
        "mov fs, ax\n\t"
        "pop rax\n\t"
        "mov es, ax\n\t"
        "pop rax\n\t"
        "mov ds, ax\n\t"
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

        "iretq\n\t"
        );
}