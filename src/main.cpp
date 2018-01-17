#include <VGAConsole.hpp>
#include <arch/x86/IO.hpp>
#include <arch/x86/IDT.hpp>
#include <arch/x86/FaultHandler.hpp>

#include <libk/stdio.h>
#include <libk/stdlib.h>
#include <libk/panic.h>

using namespace annos;

extern "C" void __cxa_pure_virtual()
{
    asm ("cli; hlt");
}

extern "C"
int kernel_main(void) {

    VGAConsole v;
    v.Clear();
    v.WriteVGA("annos v0.0.1\n", BaseColors::LightBlue);
    v.WriteVGA("Copyright (C) 2018 Arthur M\n");

    init_stdio(&v);
    kprintf("This is a test of printf: %8d %08x %s\n",
	    0x100, 0x100, "100a");

    ::x86::IDT idt;
    idt.Register();

    ::x86::FaultHandler::Init(&idt);
    
    kprintf("IDT ok\n");
    asm volatile("sti");
    

    for (;;)
	asm volatile("hlt");

    return 0xdeadbeef;
}
