#include <VGAConsole.hpp>
#include <arch/x86/IO.hpp>

#include <libk/stdio.h>
#include <libk/stdlib.h>

using namespace annos;

void _assert(int expr, const char* file, int line)
{
    if (!expr) {
	VGAConsole v;
	v.WriteVGA("Assertion failed at ");
	v.WriteVGA(file);
	v.WriteVGA(":");

	char strline[32];
	itoa(line, strline);
	
	v.WriteVGA(strline); 
	v.WriteVGA(". System halted");
	    
	asm volatile("cli; hlt");
    }
}

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

    char itoa_test[8];
    memset((void*)itoa_test, 0, 7);
    
    itoa(0x2480, itoa_test);
    v.WriteVGA((const char*)itoa_test);

    v.WriteVGA(" - ", BaseColors::LightMagenta);
    itoa(atoi("200"), itoa_test);
    v.WriteVGA((const char*)itoa_test);

    init_stdio(&v);
    kprintf("This is a test of printf: %8d %08x %s",
	    0x100, 0x100, "100a");
    
    return 0xdeadc0de + a + b;
}
