#include <VGAConsole.hpp>
#include <arch/x86/IO.hpp>

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
    int a = 2;
    int b = 2;

    VGAConsole v;
    v.Clear();
    v.WriteVGA("annos v0.0.1\n", BaseColors::LightBlue);
    v.WriteVGA("Copyright (C) 2018 Arthur M\n");

    init_stdio(&v);
    kprintf("This is a test of printf: %8d %08x %s",
	    0x100, 0x100, "100a");

    panic("test");
    _assert(0 == 1, __FILE__, __LINE__);
    
    return 0xdeadc0de + a + b;
}
