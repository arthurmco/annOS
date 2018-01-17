#include <VGAConsole.hpp>
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
    asm ("cli; hlt");
}

extern "C"
void kernel_main(void) {

    VGAConsole v;
    v.Clear();
    v.WriteVGA("annos v0.0.1\n", BaseColors::LightBlue);
    v.WriteVGA("Copyright (C) 2018 Arthur M\n");

    init_stdio(&v);
    Log::Init(&v);
    
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

    Log::Write(LogLevel::Error, "test", "Notice");
    
    asm volatile("sti");
    
    for (;;) {
	asm volatile("hlt");
    }

}
