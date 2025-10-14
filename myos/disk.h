#ifndef __DISK_H__
#define __DISK_H__
#include "size.h"
#include "ahci.h"
#define SECTOR_SIZE 512
class Disk {
private:
	uint32_t index = 0;
	uint8_t* buffer;
	volatile HBA_PORT* port;
	bool ready = false;
public:
	Disk(volatile HBA_PORT* port, uint8_t* buffer);
	uint8_t operator[](uint64_t addr);
	void read_bytes(uint64_t addr, void* buf, uint64_t size);
};
#endif /*__DISK_H__*/