#ifndef __MEMORY_H__
#define __MEMORY_H__

__attribute__((no_caller_saved_registers))
static inline void* memcpy(void* dest, const void* src, unsigned long long size) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    while (size--) {
        *d++ = *s++;
    }

    return dest;
}

__attribute__((no_caller_saved_registers))
static inline void* memset(void* dest, int value, unsigned long long size) {
    unsigned char* d = (unsigned char*)dest;
    while (size--) {
        *d++ = (unsigned char)value;
    }
    return dest;
}
#endif /* __MEMORY_H__ */