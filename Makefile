# annos makefile
# Copyright (C) 2018 Arthur M

CXX=/usr/local/gcc-7.2.0/bin/i686-elf-g++
AS=/usr/local/gcc-7.2.0/bin/i686-elf-as

CXXFLAGS= -std=gnu++14 -ffreestanding -nostdlib -Wall -m32 -fno-exceptions -fno-rtti
CXXINCLUDES= -I$(CURDIR)/src/include
LDFLAGS=-lgcc -g

OUT=annos.elf

# List of files, by group
CXX_CTORS_START = src/boot/x86/crti.o \
		  $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtbegin.o)
CXX_CTORS_END = $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtend.o) \
		src/boot/x86/crtn.o

X86_ARCH = src/boot/x86/entry.o src/arch/x86/IO.o

KERNEL_COMMON= src/main.o src/VGAConsole.o src/Device.o

LIBK_COMMON= src/libk/stdlib.o src/libk/stdio.o
# List of targets

all: annos

qemu-run-debug: annos
	echo "Debugger ready at localhost:1234"
	qemu-system-i386 -kernel $(OUT) -serial stdio -s -S

qemu-run: annos
	qemu-system-x86_64 -kernel $(OUT) -serial stdio

iso: annos
	cp $(OUT) iso/boot
	grub-mkrescue -o $(OUT).iso iso/

annos: $(CXX_CTORS_START) $(X86_ARCH) $(KERNEL_COMMON) $(CXX_CTORS_END) $(LIBK_COMMON)
	$(CXX) -T linker.ld -o $(OUT) $(CXXINCLUDES) $(CXXFLAGS) -lgcc $^ $(LDFLAGS)

%.o: %.S
	$(AS) -o $@ $< $(ASMFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXINCLUDES) $(CXXFLAGS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete

