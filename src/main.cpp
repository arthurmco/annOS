#include <VGAConsole.hpp>
#include <arch/x86/IO.hpp>

using namespace annos;

extern "C" void __cxa_pure_virtual()
{
    asm ("cli; hlt");
}

void update_cursor() {
    ::x86::out8(0x3d4, 0x0f);
    ::x86::out8(0x3d5, 2);
    ::x86::out8(0x3d4, 0x0e);
    ::x86::out8(0x3d5, 0);    
}

void write_letter() {
    unsigned short* fb = (unsigned short*)0xb8000;
    fb[0] = (unsigned short)'H' | (unsigned short) 0xf0 << 8;
}

extern "C"
int kernel_main(void) {
    int a = 2;
    int b = 2;

    write_letter();
    
    VGAConsole v;
    v.Clear();
    v.WriteVGA("annos v0.0.1\n", BaseColors::LightBlue);
    v.WriteVGA("Copyright (C) 2018 Arthur M\n");
    update_cursor();  
    
    return 0xdeadc0de + a + b;
}
