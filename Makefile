# annos makefile
# Copyright (C) 2018 Arthur M

CXX=/usr/local/gcc-7.2.0/bin/i686-elf-g++
AS=/usr/local/gcc-7.2.0/bin/i686-elf-as

CXXFLAGS= -std=gnu++14 -ffreestanding -nostdlib -Wall -m32 -fno-exceptions -fno-rtti

CXXINCLUDES= -I$(CURDIR)/src/include
LDFLAGS=-lgcc -g

OUT=annos.elf

# List of files, by group
CXX_CTORS_START = crt0.o crti.o $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtbegin.o)
CXX_CTORS_END = $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtend.o) crtn.o

X86_ARCH = src/boot/x86/entry.o

KERNEL_COMMON= src/main.o
# List of targets

all: annos

annos: $(X86_ARCH) $(KERNEL_COMMON)
	$(CXX) -T linker.ld -o $(OUT) $(CXXFLAGS) -lgcc $^ $(LDFLAGS)

%.o: %.S
	$(AS) -o $@ $< $(ASMFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete

