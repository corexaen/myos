#include "VirtPageAllocator"
#include "memory.h"
#include "log.h"
volatile uint32_t vpaspinv = 0;
inline void _lockv() {
    while (__atomic_test_and_set(&vpaspinv, __ATOMIC_ACQUIRE));
}
inline void _unlockv() {
    __atomic_clear(&vpaspinv, __ATOMIC_RELEASE);
}
inline void VirtPageAllocator::invlpg(void* addr) {
    asm volatile("invlpg [%0]" :: "r"(addr) : "memory");
}
VirtPageAllocator::VirtPageAllocator() = default;
void VirtPageAllocator::init(PhysPageAllocator* _phy_allocator) {
    uint64_t cr3;
    phy_allocator = _phy_allocator;
    asm volatile ("mov %0, cr3" : "=r"(cr3));
    // CR3 하위 12비트(PCID/플래그) 제거 + HHDM으로 가상화
    uint64_t pml4_pa = cr3 & ~0xFFFULL;
    //pml4 = (void*)(HHDM_BASE + pml4_pa);
    pml4 = (void*)pml4_pa;
}
uint64_t VirtPageAllocator::alloc_virt_page(uint64_t va, uint64_t pa, uint64_t flags) {
    if ((va & 0xFFF) || (pa & 0xFFF)) return ~0ULL; // 정렬 불량

    _lockv();

    // PML4E
    uint64_t* pml4e = (uint64_t*)pml4 + ((va >> 39) & 0x1FF);
    if (!(*pml4e & P)) {
        uint64_t new_pdpt_pa = phy_allocator->alloc_phy_page();
        if (!new_pdpt_pa) { _unlockv(); return ~0ULL; }
        memset((void*)(HHDM_BASE + new_pdpt_pa), 0, 4096);
        *pml4e = (new_pdpt_pa & ~0xFFFULL) | RW | P; // User=0(커널)
    }

    // PDPTE
    uint64_t* pdpte = (uint64_t*)(HHDM_BASE + (*pml4e & ~0xFFFULL)) + ((va >> 30) & 0x1FF);
    if (*pdpte & PS) { _unlockv(); return ~0ULL; } // 1GiB 페이지 충돌
    if (!(*pdpte & P)) {
        uint64_t new_pd_pa = phy_allocator->alloc_phy_page();
        if (!new_pd_pa) { _unlockv(); return ~0ULL; }
        memset((void*)(HHDM_BASE + new_pd_pa), 0, 4096);
        *pdpte = (new_pd_pa & ~0xFFFULL) | RW | P;
    }

    // PDE
    uint64_t* pde = (uint64_t*)(HHDM_BASE + (*pdpte & ~0xFFFULL)) + ((va >> 21) & 0x1FF);
    if (*pde & PS) { _unlockv(); return ~0ULL; } // 2MiB 페이지 충돌
    if (!(*pde & P)) {
        uint64_t new_pt_pa = phy_allocator->alloc_phy_page();
        if (!new_pt_pa) { _unlockv(); return ~0ULL; }
        memset((void*)(HHDM_BASE + new_pt_pa), 0, 4096);
        *pde = (new_pt_pa & ~0xFFFULL) | RW | P;
    }

    // PTE
    uint64_t* pte = (uint64_t*)(HHDM_BASE + (*pde & ~0xFFFULL)) + ((va >> 12) & 0x1FF);
    if (*pte & P) { _unlockv(); return ~0ULL; } // 이미 매핑됨 (구분하려면 별도 코드 사용)

    *pte = (pa & ~0xFFFULL) | flags;

    invlpg((void*)va); // TLB flush (해당 VA만)

    _unlockv();
    return va;
}
uint64_t VirtPageAllocator::alloc_virt_pages(uint64_t va, uint64_t pa, uint64_t size, uint64_t flags) {
    if ((va & 0xFFF) || (pa & 0xFFF) || (size & 0xFFF)) return ~0ULL; // 정렬 불량
    uint64_t pages = (size + 4095) / 4096;
    uart_print("size:");
    uart_print(size);
    uart_print("\npages:");
    uart_print(pages);
    uart_print("\n");
    for (uint64_t i = 0; i < pages; i++) {
        if (alloc_virt_page(va + i * 4096, pa + i * 4096, flags) == ~0ULL) {
            // 실패 시 지금까지 할당된 페이지 해제
            uart_print("error!!\n");
            free_virt_pages(va, i * 4096);
            return ~0ULL;
        }
    }
    return va;
}
void VirtPageAllocator::free_virt_page(uint64_t va) {
    if (va & 0xFFF) return; // 정렬 불량
    _lockv();
    // PML4E
    uint64_t* pml4e = (uint64_t*)pml4 + ((va >> 39) & 0x1FF);
    if (!(*pml4e & P)) { _unlockv(); return; } // 미할당
    // PDPTE
    uint64_t* pdpte = (uint64_t*)(HHDM_BASE + (*pml4e & ~0xFFFULL)) + ((va >> 30) & 0x1FF);
    if (!(*pdpte & P)) { _unlockv(); return; } // 미할당
    if (*pdpte & PS) { _unlockv(); return; } // 1GiB 페이지 충돌
    // PDE
    uint64_t* pde = (uint64_t*)(HHDM_BASE + (*pdpte & ~0xFFFULL)) + ((va >> 21) & 0x1FF);
    if (!(*pde & P)) { _unlockv(); return; } // 미할당
    if (*pde & PS) { _unlockv(); return; } // 2MiB 페이지 충돌
    // PTE
    uint64_t* pte = (uint64_t*)(HHDM_BASE + (*pde & ~0xFFFULL)) + ((va >> 12) & 0x1FF);
    if (!(*pte & P)) { _unlockv(); return; } // 미할당
    *pte = 0; // 해제
    invlpg((void*)va); // TLB flush (해당 VA만)
    _unlockv();
}

void VirtPageAllocator::free_virt_pages(uint64_t va, uint64_t size) {
    if (va & 0xFFF || size & 0xFFF) return; // 정렬 불량
    uint64_t pages = (size + 4095) / 4096;
    for (uint64_t i = 0; i < pages; i++) {
        free_virt_page(va + i * 4096);
    }
}
