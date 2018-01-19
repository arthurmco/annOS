#include <VGAConsole.hpp>
#include <DebugConsole.hpp>
#include <Log.hpp>
#include <arch/x86/IO.hpp>
#include <arch/x86/IDT.hpp>
#include <arch/x86/PIT.hpp>
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


/* Memory map multiboot field */
struct MultibootMmap {
    uint32_t size;
    struct {
	uint64_t addr;
	uint64_t len;
	uint32_t type;
	uint32_t size;
    } map[];
};

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
    kprintf("\t-> Bootloader flags: \033[1m0x%08x\033[0m\n", bif->flags);
    kprintf("\t-> Memory total: \033[1m%d kB\033[0m\n",
	    bif->mem_lower+bif->mem_upper);
    kprintf("\t-> Boot device: \033[1m %x \033[0m\n",
	    bif->boot_device);
    kprintf("\t-> Command line: \033[1m%s\033[0m\n",
	    ((const char*)bif->cmdline));
    kprintf("\t-> \033[1m%d\033[0m boot modules, pointer at 0x%08x\n",
	    bif->mods_count, bif->mods_addr);
    kprintf("\t-> Memory map pointer: at \033[1m0x%08x\033[0m, with "
	    "\033[1m%d\033[0m bytes\n",
	    bif->mmap_addr, bif->mmap_length);
	    

    MultibootMmap* mb_mmap = (MultibootMmap*)bif->mmap_addr;
    Log::Write(Info, "mmap", "mmap entries are %d bytes",
	       mb_mmap->size);
    
    /* Entry count minus the initial size */
    int entcount = (bif->mmap_length - sizeof(uint32_t)) / mb_mmap->size;
    for (int i = 0; i < entcount; i++) {

	static const char* strType[] = {"", "available", "reserved",
					"ACPI", "ACPI NVS",
					"badram"};
					

	auto mtype = mb_mmap->map[i].type;
	Log::Write(Info, "mmap",
		   "\t %d: start 0x%08x len 0x%08x type %d (%s)", i,
		   (uint32_t)mb_mmap->map[i].addr,
		   (uint32_t)mb_mmap->map[i].len,
		   mtype, strType[(mtype > 5) ? 2 : mtype]);
    }
    
    ::x86::PIT p;
    p.Initialize();
    ::x86::IRQHandler::SetHandler(0, &p);
    
    
    for (;;) {
	asm volatile("hlt");
    }

}
