#include "lapic.h"
#include "allocator"
#include "msr.h"
#include "io.h"
volatile uint64_t lapic_base;

void init_lapic_base() {
    lapic_base = rdmsr(0x1B) & 0xFFFFF000;  // ���� 12��Ʈ�� ����
    virt_page_allocator->alloc_virt_page(lapic_base, lapic_base, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::PCD);
}

// PIC ��Ȱ��ȭ
void disable_pic() {
    outb(0xA1, 0xFF); // �����̺� PIC ����ũ
    outb(0x21, 0xFF); // ������ PIC ����ũ
}

// Local APIC Ȱ��ȭ
void enable_apic() {
    // ���� lapic_base ���
    uint64_t val = rdmsr(0x1B);
    val |= (1 << 11);  // APIC Global Enable
    wrmsr(0x1B, val);

    volatile uint32_t* lapic = (volatile uint32_t*)lapic_base;
    lapic[0xF0 / 4] = (lapic[0xF0 / 4] & 0xFFFFFDFF) | 0x100;
}
// IOAPIC �������� ����

static inline void lapic_write(uint32_t reg, uint32_t value) {
    *(volatile uint32_t*)(lapic_base + reg) = value;
}
#define LAPIC_REG_TIMER         0x320
#define LAPIC_TIMER_MODE_PERIODIC (1 << 17)
#define LAPIC_REG_TIMER_DIVIDE 0x3E0
#define LAPIC_REG_TIMER_INIT   0x380
void setup_lapic_timer(uint8_t vector) {
    // 1. Divide Configuration (Divide by 16)
    lapic_write(LAPIC_REG_TIMER_DIVIDE, 0b0011); // Divide by 16

    // 2. Set LVT Timer Register (���ͷ�Ʈ ���� ����)
    lapic_write(LAPIC_REG_TIMER, LAPIC_TIMER_MODE_PERIODIC | vector);

    // 3. Set Initial Count (Ŭ�� ����Ŭ ��)
    lapic_write(LAPIC_REG_TIMER_INIT, 0x100000); // �� �۰� = ����, ũ�� = ����
}