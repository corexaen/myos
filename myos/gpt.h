#ifndef __GPT_H__
#define __GPT_H__
#include "size.h"
#include "ahci.h"
uint16_t init_gpt(volatile HBA_PORT* port, void* header);
#endif