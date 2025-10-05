#include "pci.h"
#include "kernel.h"
#include "io.h"
#include "size.h"
#include "allocator"
#include "log.h"
#include "memory.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32_t pci_config_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset) {
	uint32_t address;
	// Create configuration address as per Figure 1
	address = (uint32_t)((bus << 16) | (slot << 11) |
						(func << 8) | (offset & 0xfc) | (1 << 31));
	// Write out the address
	outl(PCI_CONFIG_ADDRESS, address);
	// Read in the data
	// (offset & 2) * 8) = 0 will choose the first word of the 32 bits register
	return inl(PCI_CONFIG_DATA);
}

void pci_config_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
	uint32_t address;
	// Create configuration address as per Figure 1
	address = (uint32_t)((bus << 16) | (slot << 11) |
						(func << 8) | (offset & 0xfc) | (1 << 31));
	// Write out the address
	outl(PCI_CONFIG_ADDRESS, address);
	// Write the data
	// (offset & 2) * 8) = 0 will choose the first word of the 32 bits register
	outl(PCI_CONFIG_DATA, value);
}
typedef struct {
    uint64_t addr;  // BAR 주소
    uint64_t size;  // BAR 크기
    int is_mmio;    // 1 = MMIO, 0 = I/O
    int is_64;      // 1 = 64-bit BAR
} pci_bar_info_t;


//todo: 32를 위한것도 만들기
pci_bar_info_t pci_get_bar_size(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    pci_bar_info_t info;

    uint32_t old_low = pci_config_read(bus, slot, func, offset);
    uint32_t old_high = 0;

    int is_io = old_low & 0x1;
    int is_64 = (!is_io && (old_low & 0x4));  // bit2=1 → 64-bit BAR

    if (is_64) { 
        old_high = pci_config_read(bus, slot, func, offset + 4);
		uart_print("64bit BAR detected\n");
    }
    else {
		uart_print("32bit BAR detected\n");
    }

    // probe
    pci_config_write32(bus, slot, func, offset, 0xFFFFFFFF);
    if (is_64)
        pci_config_write32(bus, slot, func, offset + 4, 0xFFFFFFFF);

    uint32_t val_low = pci_config_read(bus, slot, func, offset);
    uint32_t val_high = is_64 ? pci_config_read(bus, slot, func, offset + 4) : 0;

    // restore
    pci_config_write32(bus, slot, func, offset, old_low);
    if (is_64)
        pci_config_write32(bus, slot, func, offset + 4, old_high);

    // 주소 계산
    if (is_io) {
        info.addr = old_low & 0xFFFFFFFCULL;
    }
    else {
        uint64_t addr = old_low & 0xFFFFFFF0ULL;
        if (is_64) addr |= ((uint64_t)old_high << 32);
        info.addr = addr;
    }

    // 크기 계산
    if (is_io) {
        uint32_t mask = val_low & 0xFFFFFFFC;
        info.size = ~(mask)+1;
		uart_print("IO BAR size calculated\n");
    }
    else {
        uint64_t mask = ((uint64_t)val_high << 32) | (val_low & 0xFFFFFFF0ULL);
        if(is_64)
            info.size = ~(mask)+1;
		else
            info.size = ~((uint32_t)mask)+1;
		uart_print("MMIO BAR size calculated\n");
    }

    info.is_mmio = !is_io;
    info.is_64 = is_64;
    return info;
}
HBA_MEM* pci_init() {
    pci_bar_info_t abar_info = pci_get_bar_size(
        bootinfo->bootdev.pci_bus,
        bootinfo->bootdev.pci_slot,
        bootinfo->bootdev.pci_func,
        0x24   // BAR5 low offset
    );
	virt_page_allocator->alloc_virt_pages(abar_info.addr,abar_info.addr, (abar_info.size + 0xFFF) & ~0xFFFULL, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::PCD);
	probe_ports((HBA_MEM*)(abar_info.addr));
	return (HBA_MEM*)(abar_info.addr);
}