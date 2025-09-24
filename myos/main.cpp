extern "C" void main();
#ifdef _MSC_VER
#define NAKED __declspec(naked)
#define __attribute__(packed)
#define __asm__ __asm
#else
#define NAKED __attribute__((naked))
#endif
extern "C" NAKED void _start() {
    __asm__ __volatile__(
        "jmp main;"
        "hlt;"
    );
}
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed short int16_t;
typedef signed char int8_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef struct {
    uint64_t framebufferAddr;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
    uint32_t framebufferPitch;
    uint8_t* physbm;
	uint64_t physbm_size;
    void* rsdp;
} BootInfo;
static int seed = 123456789;
static inline int simple_rand() {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}
unsigned char font8x8_basic[128][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0000 (nul)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0001
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0002
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0003
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0004
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0005
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0006
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0007
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0008
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0009
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0010
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0011
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0012
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0013
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0014
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0015
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0016
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0017
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0018
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0019
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020 (space)
    { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
    { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
    { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
    { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
    { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
    { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
    { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
    { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
    { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
    { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
    { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
    { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
    { 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00},   // U+0030 (0)
    { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
    { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
    { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
    { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
    { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
    { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
    { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
    { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
    { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
    { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (;)
    { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
    { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
    { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
    { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
    { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
    { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
    { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
    { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
    { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
    { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
    { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
    { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
    { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
    { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
    { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
    { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
    { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
    { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
    { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
    { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
    { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
    { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
    { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
    { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
    { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
    { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
    { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
    { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
    { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
    { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
    { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
    { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
    { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
    { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
    { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
    { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
    { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
    { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
    { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
    { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
    { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
    { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
    { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
    { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
    { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
    { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
    { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
    { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
    { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
    { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
    { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
    { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
    { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    // U+007F
};
extern "C"
void* memcpy(void* dest, const void* src, unsigned long count) {
    char* dst8 = (char*)dest;
    const char* src8 = (const char*)src;
    while (count--) {
        *dst8++ = *src8++;
    }
    return dest;
}
extern "C"
void* memset(void* dest, int val, unsigned long count) {
    unsigned char* ptr = (unsigned char*)dest;
    while (count--) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}
volatile uint32_t ppaspinv = 0;
volatile uint32_t vpaspinv = 0;
inline void _lockp() {
	while (__atomic_test_and_set(&ppaspinv, __ATOMIC_ACQUIRE));
}
inline void _unlockp() {
    __atomic_clear(&ppaspinv, __ATOMIC_RELEASE);
}
inline void _lockv() {
    while (__atomic_test_and_set(&vpaspinv, __ATOMIC_ACQUIRE));
}
inline void _unlockv() {
    __atomic_clear(&vpaspinv, __ATOMIC_RELEASE);
}
#define AllocateAnyPages 0
#define PageSize 4096
class PhysPageAllocator {
    private:
    uint64_t total_pages;
	uint64_t used_pages = 0;
	uint8_t* bitmap;
public:
    uint64_t alloc_phy_page() {
        _lockp();
        for (uint64_t i = 0; i < (total_pages + 63) / 64; i++) {
            if (bitmap[i] != 0xFF) { // 아직 빈 페이지가 있음
                for (int j = 0; j < 8; j++) {
                    if (!(bitmap[i] & (1ULL << j))) { // 빈 페이지 발견
                        bitmap[i] |= (1ULL << j);
                        used_pages++;
                        _unlockp();
                        return (i * 64 + j) * 4096;
                    }
                }
            }
        }
        _unlockp();
        return 0; // 더 이상 할당할 페이지가 없음
    }
    void free_phy_page(uint64_t addr) {
        _lockp();
        uint64_t page_index = addr / 4096;
        uint64_t i = page_index / 64;
        uint64_t j = page_index % 64;
        if (bitmap[i] & (1ULL << j)) { // 이미 할당된 페이지인지 확인
            bitmap[i] &= ~(1ULL << j);
            used_pages--;
        }
        _unlockp();
    }
    PhysPageAllocator() = default;
    void init(uint8_t* _bitmap, uint64_t _total_pages) {
        total_pages = _total_pages;
        bitmap = _bitmap;
    }
    uint64_t get_total_pages() const { return total_pages; }
    uint64_t get_used_pages() const { return used_pages; }
	uint64_t get_free_pages() const { return total_pages - used_pages; }
};
#define HHDM_BASE 0xFFFFFF0000000000ULL
class VirtPageAllocator {
private:
    static inline void invlpg(void* addr) {
        asm volatile("invlpg [%0]" :: "r"(addr) : "memory");
    }
public:
    PhysPageAllocator* phy_allocator;
    static constexpr uint64_t P = 1ull << 0;
    static constexpr uint64_t RW = 1ull << 1;
    static constexpr uint64_t US = 1ull << 2;
	static constexpr uint64_t PWT = 1ull << 3;
	static constexpr uint64_t PCD = 1ull << 4;
	static constexpr uint64_t A = 1ull << 5;
	static constexpr uint64_t D = 1ull << 6;
    static constexpr uint64_t PS = 1ull << 7;
	static constexpr uint64_t G = 1ull << 8;

    static constexpr uint64_t NX = 1ull << 63;
    VirtPageAllocator() = default;
    void* pml4; // 커널 가상주소(= HHDM + PML4 물리)로 보관
    void init(PhysPageAllocator* _phy_allocator) {
        uint64_t cr3;
		phy_allocator = _phy_allocator;
        asm volatile ("mov %0, cr3" : "=r"(cr3));
        // CR3 하위 12비트(PCID/플래그) 제거 + HHDM으로 가상화
        uint64_t pml4_pa = cr3 & ~0xFFFULL;
        //pml4 = (void*)(HHDM_BASE + pml4_pa);
		pml4 = (void*)pml4_pa;
    }
    uint64_t alloc_virt_page(uint64_t va, uint64_t pa,uint64_t flags) {
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
};
inline void* operator new(unsigned long, void* p) noexcept { return p; }
inline void* operator new[](unsigned long, void* p) noexcept { return p; }
alignas(PhysPageAllocator) static uint8_t phy_buf[sizeof(PhysPageAllocator)];
PhysPageAllocator* phy_page_allocator;

alignas(VirtPageAllocator) static uint8_t virt_buf[sizeof(VirtPageAllocator)];
VirtPageAllocator* virt_page_allocator;

void init_allocators(uint8_t* bitmap, uint64_t total_pages) {
    phy_page_allocator = new (phy_buf) PhysPageAllocator();
    phy_page_allocator->init(bitmap, total_pages);

    virt_page_allocator = new (virt_buf) VirtPageAllocator();
    virt_page_allocator->init(phy_page_allocator);
}
void putc(BootInfo* f, int x, int y, char text, uint32_t color, int scale) {
    if (text < 32 || text > 127) return;
    for (int row = 0; row < 8; row++) {
        uint8_t bits = font8x8_basic[(int)text][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << col)) {
                for (int y_offset = 0; y_offset < scale * 2; y_offset++) {
                    for (int x_offset = 0; x_offset < scale; x_offset++) {
                        ((uint32_t*)(f->framebufferAddr))[(int)f->framebufferPitch * (y + row * scale * 2 + y_offset) + (x + col * scale + x_offset)] = color;
                    }
                }
            }
        }
    }
}
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__(
        "mov dx, %0;"
        "mov al, %1;"
        "out dx, al;"
        :
    : "r" (port), "r" (value)
        : "dx", "al"
        );
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__(
        "mov dx, %1;"
        "xor eax, eax;"
        "in al, dx;"
        "mov %0, al;"
        : "=r" (ret)
        : "r" (port)
        : "eax", "edx"
    );
    return ret;
}
#define IDT_SIZE 256
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
idt_entry idt[IDT_SIZE] __attribute__((aligned(16)));
idt_ptr idt_reg;
void set_idt_gate(int n, uint64_t handler, uint16_t selector, uint8_t flags) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].offset_middle = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].selector = selector;
    idt[n].zero = 0;
    idt[n].type_attr = flags;
	idt[n].reserved = 0;
}
void _load_idt(idt_ptr* _idt_ptr) {
    __asm__ __volatile__(
        "lidt [%0];"
        : : "r"(_idt_ptr) : "memory"
    );
}
void load_idt() {
    idt_reg.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idt_reg.base = (uint64_t)&idt;
    _load_idt(&idt_reg);
}
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    __asm__ volatile ("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

// IOAPIC 레지스터 접근
#define IOAPIC_BASE  0xFEC00000
uint64_t lapic_base;

void init_lapic_base() {
    lapic_base = rdmsr(0x1B) & 0xFFFFF000;  // 하위 12비트는 무시
	virt_page_allocator->alloc_virt_page(IOAPIC_BASE, IOAPIC_BASE, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::PCD);
	virt_page_allocator->alloc_virt_page(lapic_base, lapic_base, VirtPageAllocator::P | VirtPageAllocator::RW | VirtPageAllocator::PCD);
}
static inline void ioapic_write(uint8_t reg, uint32_t value) {
    volatile uint32_t* ioapic = (volatile uint32_t*)IOAPIC_BASE;
    ioapic[0] = reg;
    ioapic[4] = value;
}

static inline uint32_t ioapic_read(uint8_t reg) {
    volatile uint32_t* ioapic = (volatile uint32_t*)IOAPIC_BASE;
    ioapic[0] = reg;
    return ioapic[4];
}

// PIC 비활성화
void disable_pic() {
    outb(0xA1, 0xFF); // 슬레이브 PIC 마스크
    outb(0x21, 0xFF); // 마스터 PIC 마스크
}

// Local APIC 활성화
void enable_apic() {
    // 전역 lapic_base 사용
    uint64_t val = rdmsr(0x1B);
    val |= (1 << 11);  // APIC Global Enable
    wrmsr(0x1B, val);

    volatile uint32_t* lapic = (volatile uint32_t*)lapic_base;
    lapic[0xF0 / 4] = (lapic[0xF0 / 4] & 0xFFFFFDFF) | 0x100;
}
void ioapic_set_redirection(uint8_t irq, uint8_t vector, uint8_t apic_id) {
    uint8_t reg = 0x10 + irq * 2;        // LOW
    uint8_t reg_high = reg + 1;          // HIGH

    // HIGH: destination APIC ID (target CPU)
    uint32_t high = ((uint32_t)apic_id) << 24;

    // LOW 설정
    uint32_t low = 0;
    low |= vector;           // Vector (IDT entry number)
    low |= (0 << 8);         // Delivery Mode = Fixed
    low |= (0 << 11);        // Destination Mode = Physical
    low |= (0 << 13);        // Polarity = Active High
    low |= (0 << 15);        // Trigger Mode = Edge
    low |= (0 << 16);        // Mask = 0 (enable)

    // IOAPIC에 쓰기
    ioapic_write(reg_high, high);  // Destination CPU
    ioapic_write(reg, low);        // Redirection Entry Low
}
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

    // 2. Set LVT Timer Register (인터럽트 벡터 설정)
    lapic_write(LAPIC_REG_TIMER, LAPIC_TIMER_MODE_PERIODIC | vector);

    // 3. Set Initial Count (클럭 사이클 수)
    lapic_write(LAPIC_REG_TIMER_INIT, 0x100000); // 값 작게 = 빠름, 크면 = 느림
}
// 최종 APIC 초기화
void init_apic() {
    disable_pic();               // 1. PIC 끄고
    init_lapic_base();
    enable_apic();               // 2. Local APIC 켜고
    setup_lapic_timer(32);
    ioapic_set_redirection(1, 0x21, 0);
    ioapic_set_redirection(12, 0x2C, 0);
}
#define LAPIC_EOI_REGISTER  0xB0
static inline void* simple_memcpy(void* dest, const void* src, unsigned long size) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    while (size--) {
        *d++ = *s++;
    }

    return dest;
}
static inline void* simple_memset(void* dest, int value, unsigned long size) {
	unsigned char* d = (unsigned char*)dest;
	while (size--) {
		*d++ = (unsigned char)value;
	}
	return dest;
}
BootInfo* gGraphicsInfo = (BootInfo*)0xFFFFFFFF00200000ull;
int i1 = 0;
int i2 = 0;
//task test
extern "C" NAKED void simple_hlt() {
    __asm__ __volatile__(
        "hlt\n\t"
		"jmp simple_hlt\n\t"
    );
}
extern "C" __attribute__((noreturn, noinline)) void task1_func() {
    int a = 0;
    int b;
    while (1) {
        for (i1 = 0; i1 < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight / 2; i1++) {
            *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i1) = a << 16;
        }
        a = (a + 1) % 0x100;
        b++;
    }
}
extern "C" __attribute__((noreturn, noinline)) void task2_func() {
    int a = 0;
    int c;
    while (1) {
        for (i2 = gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight / 2; i2 < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i2++) {
            *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i2) = a;
        }
        a = (a + 1) % 0x100;
        c++;
    }
}
typedef struct __context_t {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} context_t;
context_t* current;
context_t* next;
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
static inline void lapic_eoi() {
    *(volatile uint32_t*)(lapic_base + LAPIC_EOI_REGISTER) = 0;
}

#define COM1 0x3F8
void uart_init() {
    /*
    outb(COM1 + 1, 0x00);    // Disable interrupts
    outb(COM1 + 3, 0x80);    // Enable DLAB
    outb(COM1 + 0, 0x03);    // Baud divisor (38400 baud)
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop
    outb(COM1 + 2, 0xC7);    // Enable FIFO
    outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    */
}

int uart_is_transmit_empty() {
    //return inb(COM1 + 5) & 0x20;
}

void uart_putc(char c) {
    //while (!uart_is_transmit_empty());
    //outb(COM1, c);
}

void uart_print(const char* s) {
    //while (*s) {
    //    if (*s == '\n') uart_putc('\r');
    //    uart_putc(*s++);
    //}
}
uint8_t console[100 * 40] = { 0, };
static const char hex_digits[] = "0123456789ABCDEF";

static inline void bytes_to_hex_string(const char* src, int len, char* dst) {
    for (int i = 0; i < len; i++) {
        unsigned char byte = (unsigned char)src[i];
        dst[i * 3 + 0] = hex_digits[(byte >> 4) & 0xF]; // 상위 4비트
        dst[i * 3 + 1] = hex_digits[byte & 0xF];        // 하위 4비트
        dst[i * 3 + 2] = ' ';                           // 간격용
    }
    dst[len * 3] = '\0';  // null-terminate
}
typedef struct interrupt_frame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) interrupt_frame_t;
__attribute__((interrupt)) void keyboard_handler(interrupt_frame_t* frame) {
    uint8_t scancode = inb(0x60);
    
    BootInfo* gGraphicsInfo = (BootInfo*)0xFFFFFFFF00200000ull;
    for (int i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
        uint8_t Red = 0;
        uint8_t Green = simple_rand() % 256;
        uint8_t Blue = 255;

        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = PixelColor;
    }
    
    /*
    if (console[0] == 0) {
        char raw_frame[24];
        __builtin_memcpy(raw_frame, frame, 24);
        bytes_to_hex_string(raw_frame, 24, (char*)console);
    }
    */
    lapic_eoi();
}
__attribute__((interrupt)) void dummy_mouse_handler(interrupt_frame_t* frame) {
    inb(0x60);
    BootInfo* gGraphicsInfo = (BootInfo*)0xFFFFFFFF00200000ull;
    for (int i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
        uint8_t Red = 255;
        uint8_t Green = 255;
        uint8_t Blue = 0;

        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = PixelColor;
    }
    lapic_eoi();
}
static inline uint64_t get_rsp() {
    uint64_t v;
    asm volatile ("mov %0, rsp" : "=r"(v));
    return v;
}
static inline void set_rsp(uint64_t v) {
    asm volatile ("mov rsp, %0" : : "r"(v));
}
char uart_buf[1000];
extern "C" __attribute__((noinline)) uint64_t* c_timer_handler(context_t* frame) {
    current = next;
    next = frame;
    uart_print("timer\n");
	simple_memset(uart_buf, 0, sizeof(uart_buf));
	bytes_to_hex_string((char*)current, 8 * 20, uart_buf);
	uart_print(uart_buf);

    lapic_write(0xB0, 0);  // EOI
    uint64_t* ret = (uint64_t*)current;
    asm volatile("" : "+a"(ret));
    return (uint64_t*)current;
}
__attribute__((naked)) void timer_handler() {
    asm volatile(
        // 인터럽트 진입시 트랩 프레임이 이미 스택에 있음 (rip, cs, rflags, [rsp, ss])
        // 추가적으로 전역/일반 레지스터도 저장
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
        // 이제 스택 상태:
        // [r15][r14]...[rax][RIP][CS][RFLAGS]([RSP][SS]) <- rsp

        // 스택 프레임 주소를 첫 번째 인자(rdi)로 넘기자!
        "mov rdi, rsp\n\t"
        // C로 진입! (C에서 스케줄링/컨텍스트스위칭/복원 판단)
        "call c_timer_handler\n\t"
		"mov rsp, rax\n\t"  // C 함수가 반환한 rsp로 복원
        // C 함수는 복원할 context의 trapframe/레지스터를 스택에 push해둠(다음 프로세스)
        // 여기서 복귀만 하면 됨

        // pop 순서대로 pop
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

        // 이제 트랩 프레임만 스택에 남아 있음
        // iretq로 복귀 (RIP, CS, RFLAGS, [RSP, SS])를 자동으로 pop
        "iretq\n\t"
        );
}
__attribute__((interrupt)) void test_timer_handler(interrupt_frame_t* frame) {
    lapic_eoi();
}
__attribute__((interrupt)) void none_handler(interrupt_frame_t* frame) {
    __asm__ __volatile__("hlt");
    lapic_eoi();
}
__attribute__((interrupt)) void page_fault_handler(interrupt_frame_t* frame, uint64_t error_code) {
    uint64_t cr2;
    asm volatile ("mov %0, cr2" : "=r"(cr2));
    char raw_stack[16];
    __builtin_memcpy(raw_stack, (void*)&frame->cs, 8);
	__builtin_memcpy(raw_stack + 8, (void*)&error_code, 8);
    bytes_to_hex_string(raw_stack, sizeof(raw_stack), (char*)console);
    for (int i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
        uint8_t Red = 255;
        uint8_t Green = 255;
        uint8_t Blue = 255;

        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = PixelColor;
    }
    while (1) {
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 100; x++) {
                putc(gGraphicsInfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 100 + x], 0, 1);
            }
        }
    }
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void general_protection_fault_handler(interrupt_frame_t* frame, uint64_t error_code) {
    for (int i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
        uint8_t Red = 255;
        uint8_t Green = 255;
        uint8_t Blue = 255;
        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = PixelColor;
    }
    char raw_stack[8];
    __builtin_memcpy(raw_stack, (void*)&error_code, 8);
    bytes_to_hex_string(raw_stack, sizeof(raw_stack), (char*)console);
    console[3 * 9] = 'G';
	console[3 * 9 + 1] = 'F';
    while (1) {
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 100; x++) {
                putc(gGraphicsInfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 100 + x], 0, 1);
            }
        }
    }
    __asm__ __volatile__("hlt");
}
__attribute__((interrupt)) void stack_segment_fault_handler(interrupt_frame_t* frame, uint64_t error_code) {
    char raw_stack[8];
    __builtin_memcpy(raw_stack, (void*)&error_code, 8);
    bytes_to_hex_string(raw_stack, sizeof(raw_stack), (char*)console);
	console[3 * 9] = 'S';
	console[3 * 9 + 1] = 'S';
    for (int i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
        uint8_t Red = 255;
        uint8_t Green = 255;
        uint8_t Blue = 255;
        uint32_t PixelColor = (Red << 16) | (Green << 8) | Blue;
        *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = PixelColor;
    }
    while (1) {
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 100; x++) {
                putc(gGraphicsInfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 100 + x], 0, 1);
            }
        }
    }
    __asm__ __volatile__("hlt");
}
void init_interrupts() {
    asm volatile ("cli");
    BootInfo* gBootInfo = (BootInfo*)0xFFFFFFFF00200000ull;
    uart_init();
	init_allocators(gBootInfo->physbm, gBootInfo->physbm_size);
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
    __builtin_memcpy(raw_stack, (void*)&_rsp, 8);
    bytes_to_hex_string(raw_stack, 8, (char*)console);
    __builtin_memcpy(raw_stack, (void*)_rsp, 24);
	bytes_to_hex_string(raw_stack, 24, (char*)console + 100);
    //BootInfo* gGraphicsInfo = (BootInfo*)0xFFFFFFFF00200000ull;
    //console[3] = 'B';
    //__asm__ __volatile__("hlt");
    while (1) {
		simple_memcpy(console, uart_buf, 3 * 8 * 20);
        bytes_to_hex_string((char*)next, 8 * 20, (char*)console + 7 * 96);
        for (i = 0; i < gGraphicsInfo->framebufferPitch * gGraphicsInfo->framebufferHeight; i++) {
            *((uint32_t*)(gGraphicsInfo->framebufferAddr) + i) = 0xFFFFFF;
        }
        for (y = 0; y < 40; y++) {
            for (x = 0; x < 96; x++) {
                putc(gGraphicsInfo, x * 1 * 8 + 4, y * 2 * 16 + 4, console[y * 96 + x], 0, 1);
            }
        }
		__asm__ __volatile__("hlt");
    }
}