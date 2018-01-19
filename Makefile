# annos makefile
# Copyright (C) 2018 Arthur M

CXX=/usr/local/gcc-7.2.0/bin/i686-elf-g++
AS=/usr/local/gcc-7.2.0/bin/i686-elf-as
QEMU=qemu-system-i386

CXXFLAGS= -std=gnu++14 -ffreestanding -nostdlib -Wall -m32 -fno-exceptions -fno-rtti
CXXINCLUDES= -I$(CURDIR)/src/include
LDFLAGS=-lgcc -g

OUT=annos.elf

# List of files, by group
CXX_CTORS_START = src/boot/x86/crti.S.o \
		  $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtbegin.o)
CXX_CTORS_END = $(shell $(CXX) $(CXXFLAGS) -print-file-name=crtend.o) \
		src/boot/x86/crtn.S.o

X86_ARCH = src/boot/x86/entry.S.o src/arch/x86/IO.cpp.o src/arch/x86/IDT.cpp.o \
	   src/arch/x86/IDT.S.o src/arch/x86/FaultHandler.cpp.o \
	   src/arch/x86/FaultHandler.S.o src/arch/x86/IRQHandler.cpp.o \
	   src/arch/x86/IRQHandler.S.o src/arch/x86/i8259.cpp.o \
	   src/arch/x86/PIT.cpp.o src/arch/x86/SMBIOS.cpp.o

KERNEL_COMMON= src/main.cpp.o src/VGAConsole.cpp.o src/Device.cpp.o \
	       src/Log.cpp.o src/DebugConsole.cpp.o src/Timer.cpp.o

LIBK_COMMON= src/libk/stdlib.cpp.o src/libk/stdio.cpp.o \
             src/libk/stdio_write.cpp.o src/libk/panic.cpp.o
# List of targets

all: annos

qemu-run-debug: annos
	echo "Debugger ready at localhost:1234"
	$(QEMU) -kernel $(OUT) -serial stdio -s -S

qemu-run: annos
	$(QEMU) -kernel $(OUT) -serial stdio

iso: annos
	cp $(OUT) iso/boot
	grub-mkrescue -o $(OUT).iso iso/

annos: $(CXX_CTORS_START) $(X86_ARCH) $(KERNEL_COMMON) $(CXX_CTORS_END) $(LIBK_COMMON)
	$(CXX) -T linker.ld -o $(OUT) $(CXXINCLUDES) $(CXXFLAGS) -lgcc $^ $(LDFLAGS)

%.S.o: %.S
	$(AS) -o $@ $< $(ASMFLAGS)

%.cpp.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXINCLUDES) $(CXXFLAGS) $(LDFLAGS)

clean:
	find . -name "*.o" -type f -delete

