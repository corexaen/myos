CXX = clang++
LD = ld
OBJCOPY = objcopy

CXXFLAGS = -m64 -ffreestanding -fno-rtti -fno-exceptions -fno-zero-initialized-in-bss \
           -O2 -g -masm=intel -std=c++20
LDFLAGS = -nostdlib -T linker.ld

# 소스와 오브젝트 자동 탐색
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: disk.img

# cpp -> o
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# 링크: ELF 생성
kernel.elf: $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

# ELF -> raw binary
disk.img: kernel.elf
	$(OBJCOPY) -O binary --only-section=.text --only-section=.rodata --only-section=.data \
		kernel.elf $@

clean:
	rm -f $(OBJ) kernel.elf disk.img