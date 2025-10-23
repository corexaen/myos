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
#include "pci.h"
#include "gpt.h"
#include "FAT32.h"
#include "disk.h"
#include "process.h"

//pml4
//256 partition data heap
//257 mmap table heap
//258 process message queue
//509 mmio
//510 HHDM
//511 kernel + bootdata + init stack


// ���� APIC �ʱ�ȭ
void init_apic() {
    disable_pic();               // 1. PIC ����
    init_ioapic_base();
    init_lapic_base();
    enable_apic();               // 2. Local APIC �Ѱ�
    setup_lapic_timer(32);
    ioapic_set_redirection(1, 0x21, 0);
    ioapic_set_redirection(12, 0x2C, 0);
}

void init_interrupts() {
	init_allocators(bootinfo->physbm, bootinfo->physbm_size);
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
	set_idt_gate(0x80, (uint64_t)syscall_idthandler, 0x08, 0xEE);
    load_idt();
	//asm volatile ("sti");
}
char testbuf[PageSize * 3 + 1];

//�ϴ� �ֺܼ���
extern "C" __attribute__((force_align_arg_pointer, noinline)) void main() {
    __asm__ __volatile__ ("cli");
    uart_init();
    init_tss(0, 0);
    init_interrupts();
	*(char*)(0xFFFF800000000000) = 0; // pml4 ������ ���� �Ҵ�(256��° ��Ʈ��)
	*(char*)(0xFFFF808000000000) = 0; // pml4 ������ ���� �Ҵ�(257��° ��Ʈ��)
    HBA_MEM* hba = pci_init();
    uint64_t buf_phys = phy_page_allocator->alloc_phy_page() + MMIO_BASE;
	virt_page_allocator->alloc_virt_page(buf_phys, buf_phys - MMIO_BASE, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::PCD);
	memset((void*)buf_phys, 0, PageSize);
    ahci_read(hba->ports + bootinfo->bootdev.port_or_ns, 1, 1, (void*)buf_phys);

    FAT32 fs;
	fs.init(init_gpt(hba->ports, (void*)buf_phys), 0, (void*)buf_phys);
    uint32_t filesize = fs.get_file_size("TASK.O");
	uart_print("filesize=");
	uart_print(filesize);
    uart_print("\n");
	uint64_t readbuffer = phy_page_allocator->alloc_phy_page() + HHDM_BASE;
	fs.read_file("TASK.O", (void*)readbuffer, filesize);
	bytes_to_hex_string((char*)readbuffer, filesize, testbuf);
	uart_print(testbuf);
    Process* process = new ((void*)(phy_page_allocator->alloc_phy_page() + HHDM_BASE)) Process();
    process->init(0x1B, 0x23);
    process->addCode((void*)readbuffer);
	process->setHeap();
    init_process(process);
    uart_print("\ntest\n");
    jmp_process();
}