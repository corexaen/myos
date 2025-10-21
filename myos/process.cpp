#include "process.h"
#include "memory.h"
#include "util.h"
#include "log.h"
#define PHYS_TO_HHDM(pa) ((void *)((uint64_t)(pa) + HHDM_BASE))
// todo - GPT�� �ھ�� ���� �� �� �ֵ��� ���߿� ������������� �ھ����ŭ �����������
alignas(16) static uint8_t gdt[128];
static GDTR gdtr;
static TSS64 tss;
__attribute__((noinline, optimize("O0")))
static uint64_t make_tss_desc0(uint64_t base, uint32_t limit) {
    return (limit & 0xFFFFULL)
        | ((base & 0xFFFFFFFFULL) << 16)
        | (0x89ULL << 40)
        | (((limit >> 16) & 0xF) << 48)
        | (((base >> 24) & 0xFFULL) << 56);
}
__attribute__((optimize("O0"), noinline))
void init_tss(uint64_t kernel_stack_phys, uint64_t ist1_phys) {
    memset(gdt, 0, sizeof(gdt));

    auto set_desc = [](uint8_t* dst, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
        dst[0] = limit & 0xFF;
        dst[1] = (limit >> 8) & 0xFF;
        dst[2] = base & 0xFF;
        dst[3] = (base >> 8) & 0xFF;
        dst[4] = (base >> 16) & 0xFF;
        dst[5] = access;
        dst[6] = ((limit >> 16) & 0x0F) | (flags & 0xF0);
        dst[7] = (base >> 24) & 0xFF;
        };

    // Kernel Code/Data/User Code/Data
    set_desc(&gdt[8 * 1], 0, 0, 0x9A, 0x20); // Kernel code
    set_desc(&gdt[8 * 2], 0, 0, 0x92, 0x00); // Kernel data
    set_desc(&gdt[8 * 3], 0, 0, 0xFA, 0x20); // User code
    set_desc(&gdt[8 * 4], 0, 0, 0xF2, 0x00); // User data

    memset(&tss, 0, sizeof(tss));
    tss.rsp0 = kernel_stack_phys;
    tss.ist1 = ist1_phys;
    tss.iomap_base = 0xFFFF;

    uint64_t base = (uint64_t)&tss;
    uint32_t limit = sizeof(TSS64) - 1;
    uart_print(base);
    uart_print("\n");
    volatile uint64_t desc[2]{0, 0};
    asm volatile ("" ::: "memory");
    desc[0] = make_tss_desc0(base, limit);
    asm volatile ("" ::: "memory");
    uart_print_hex(desc[0]);
    uart_print("\n");
    desc[1] = ((base >> 32) & 0xFFFFFFFFULL);
    uart_print(desc[1]);
    asm volatile("" : : "m"(desc[0]), "m"(desc[1]) : "memory");
    uart_print("\n");
    memset(((uint8_t*)&desc[1]) + 4, 0, 4); // ���� 32��Ʈ 0���� �ʱ�ȭ
    memcpy(&gdt[8 * 5], (void*)desc, 16);

    char* ch = (char*)desc;
    uart_print_hex(desc[0]);
    uart_print("\n");
    for (int i = 0; i < 16; i++) {
        uart_print_hex2(*ch);
        ch++;
    }
    gdtr.base = (uint64_t)&gdt;
    gdtr.limit = sizeof(gdt) - 1;

    __asm__ __volatile__(
        "lgdt %0\n\t"
        "mov ax, 0x10\n\t"
        "mov ds, ax\n\t"
        "mov es, ax\n\t"
        "mov ss, ax\n\t"
        "mov fs, ax\n\t"
        "mov gs, ax\n\t"
        "push 0x08\n\t"
        "lea rax, [rip + 1f]\n\t"
        "push rax\n\t"
        "retfq\n\t"
        "1:\n\t"
        : : "m"(gdtr));
    uint16_t tss_sel = 0x28;
    asm volatile("ltr %0" : : "m"(tss_sel));
}
Process* now_process = 0;
void Process::init(uint64_t cs, uint64_t ss) {
    code_va_base = 0x400000;
    pallocator = new (allocator_buffer) VirtPageAllocator();
    cr3 = phy_page_allocator->alloc_phy_page() + HHDM_BASE;
    uint64_t lcr3 = virt_page_allocator->getCr3() + HHDM_BASE;
    memcpy((void*)(cr3 + 256ull * 8), (void*)(lcr3 + 256ull * 8), 256ull * 8);
    pallocator->init(phy_page_allocator, cr3);
    user_stack_bottom = 0x00007FFFFFF000;
    user_stack_top = user_stack_bottom - PageSize * 4;
    kernel_stack_phys = phy_page_allocator->alloc_phy_page() + PageSize;
    kernel_stack = (uint64_t*)(kernel_stack_phys + HHDM_BASE);
    *(--kernel_stack) = ss;
    *(--kernel_stack) = user_stack_bottom;
    *(--kernel_stack) = 0x202; // rflags
    *(--kernel_stack) = cs;  // cs
    *(--kernel_stack) = (uint64_t)code_va_base; // rip
	mmap_table = nullptr;
    for (int i = 0; i < 15; i++) {
        *(--kernel_stack) = 0;
    }
    for (int i = 0; i < 4; i++) {
        *(--kernel_stack) = ss;
    }
    next = 0;
    state = 1;
}
void Process::addCode(void* code_addr) {
    uint64_t code = phy_page_allocator->alloc_phy_page();
    memcpy((void*)(code + HHDM_BASE), code_addr, PageSize);
    pallocator->alloc_virt_page(code_va_base, code, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::US);
    code_va_base += PageSize;
}
void Process::setHeap() {
    heap_bottom = code_va_base;
	heap_top = code_va_base;    // ���� ����ִ� ���·� ����
}
void init_process(Process* p) {
    now_process = p;
    now_process->next = now_process;
}
void jmp_process() {
    tss.rsp0 = now_process->kernel_stack_phys + HHDM_BASE;
    uint64_t now_rsp = (uint64_t)now_process->kernel_stack;
    virt_page_allocator = now_process->pallocator;
    virt_page_allocator->setCr3();
    __asm__ __volatile(
        "mov rsp, %[now_rsp]\n\t"
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
        :
        : [now_rsp]"r"(now_rsp)
    );
}
bool Process::isAddrInMMap(uint64_t va) const {
    mmap_entry* entry = mmap_table;
    while (entry) {
        if (entry->va_start <= va && va <= entry->va_end) {
            return true;
        }
        entry = entry->next;
    }
    return false;
}
volatile uint32_t mmap_lock = 0;
inline void _lockmmap() {
    while (__atomic_test_and_set(&mmap_lock, __ATOMIC_ACQUIRE)) {
		__asm__ __volatile__ ("pause");
    }
}
inline void _unlockmmap() {
    __atomic_clear(&mmap_lock, __ATOMIC_RELEASE);
}
uint64_t Process::mmap(uint64_t size, uint64_t flags) {
	if (size == 0) return ~0ULL;
	_lockmmap();
    uint64_t page_count = (size + PageSize - 1) / PageSize;
    mmap_entry* entry = mmap_table;
	mmap_entry* last = nullptr;
    while (entry != nullptr) {
        if (last == nullptr) {
            if (user_stack_top - entry->va_end > page_count * PageSize) {
                break;
            }
        }
        else {
            if (last->va_start - entry->va_end > page_count * PageSize) {
                break;
            }
        }
        last = entry;
        entry = entry->next;
    }
    if (entry == nullptr)
        if (last != nullptr)
            if (last->va_start - (page_count * PageSize) < heap_top) {
                _unlockmmap();
                return ~0ULL; // �Ҵ� �Ұ�
			}
    mmap_entry* new_entry = (mmap_entry*)MMAP_ENTRY_BASE;
    while (new_entry->flags & 0x01) {
        new_entry++;
    }
    new_entry->flags = flags | 0x1;
    if (last) {
        last->next = new_entry;
		new_entry->next = entry;
        new_entry->va_end = last->va_start - 1;
		new_entry->va_start = last->va_start - page_count * PageSize;
    } else {
        mmap_table = new_entry;
        mmap_table->next = entry;
		mmap_table->va_end = user_stack_top - 1;
		mmap_table->va_start = user_stack_top - page_count * PageSize;
    }
	_unlockmmap();
	return new_entry->va_start;
}