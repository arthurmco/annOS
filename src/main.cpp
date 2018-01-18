#include <VGAConsole.hpp>
#include <DebugConsole.hpp>
#include <Log.hpp>
#include <arch/x86/IO.hpp>
#include <arch/x86/IDT.hpp>
#include <arch/x86/i8259.hpp>
#include <arch/x86/FaultHandler.hpp>
#include <arch/x86/IRQHandler.hpp>

#include <libk/stdio.h>
#include <libk/stdlib.h>
#include <libk/panic.h>

using namespace annos;

extern "C" void __cxa_pure_virtual()
{
    panic("called virtual function without body");
}

/**
 * Boot structure
 */
struct BootStruct {
    /**
     * The multiboot magic number
     */
    uint32_t magic;

    /**
     * Physical memory pointer for the multiboot boot information structure 
     */
    uintptr_t multiboot_phys_ptr;
} __attribute((packed));

/**
 * Multiboot Boot Information Format structure
 */
struct MultibootBIF {
    // Flags used what the bootloader supports
    uint32_t flags;

    uint32_t mem_lower; // Lower memory count
    uint32_t mem_upper; // Upper memoty count

    // Boot device
    uint32_t boot_device;

    // Boot loader command line
    uint32_t cmdline;

    // Multiboot modules information
    uint32_t mods_count;
    uint32_t mods_addr;

    // Executable symbols
    struct {
	uint32_t num, size, addr, shndx;
    } elf_sym;

    // Memory map information
    uint32_t mmap_length;
    uint32_t mmap_addr;
};

extern "C"
void kernel_main(BootStruct* bs) {
    VGAConsole v;
    v.Clear();
    v.WriteVGA("annos v0.0.1\n", BaseColors::LightBlue);
    v.WriteVGA("Copyright (C) 2018 Arthur M\n");

    init_stdio(&v);

    SerialLogConsole dc;
    Log::Init(&dc);

    if (bs->magic != 0x2badb002) {
	Log::Write(LogLevel::Error, "boot",
		   "Multiboot magic number is wrong: 0x%08x != 0x2badb002",
		   bs->magic);
	panic("invalid boot magic number");
    }
    
    ::x86::IDT idt;
    idt.Register();
    kprintf("...idt ");

    ::x86::i8259 oi8259;
    oi8259.Initialize();
    kprintf("...%s ", oi8259.GetTag());

    ::x86::FaultHandler::Init(&idt);
    kprintf("...faulthandler ");

    ::x86::IRQHandler::Init(&idt, &oi8259);
    kprintf("...irqhandler\n ");
    
    asm volatile("sti");
    
    kprintf("\n Magic value is 0x%08x, multiboot is at address 0x%x\n",
	    bs->magic, bs->multiboot_phys_ptr);

    MultibootBIF* bif = (MultibootBIF*)bs->multiboot_phys_ptr;
    kprintf("\t -> Bootloader flags: \033[1m0x%08x\033[0m\n", bif->flags);
    kprintf("\t -> Memory total: \033[1m%d kB\033[0m\n",
	    bif->mem_lower+bif->mem_upper);
    kprintf("\t -> Boot device: \033[1m %x \033[0m\n",
	    bif->boot_device);
    kprintf("\t -> Command line: \033[1m%s\033[0m\n",
	    ((const char*)bif->cmdline));
	    
    
    for (;;) {
	asm volatile("hlt");
    }

}
