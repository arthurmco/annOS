#include <arch/x86/IO.hpp>


/* 
   x86 I/O port communication file

   Copyright (C) 2018 Arthur M
 */


uint8_t annos::x86::in8(uint16_t port)
{
    uint8_t val = 0;
    asm("inb %1, %0" : "=a"(val) : "Nd"(port) );

    return val;
}

void annos::x86::out8(uint16_t port, uint8_t val)
{
    
    asm("outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint16_t annos::x86::in16(uint16_t port)
{
    uint16_t val = 0;
    asm("inw %1, %0" : "=a"(val) : "Nd"(port) );

    return val;
}

void annos::x86::out16(uint16_t port, uint16_t val)
{
    asm("outw %0, %1" : : "a"(val), "Nd"(port) );
}

uint32_t annos::x86::in32(uint16_t port)
{
    uint32_t val = 0;
    asm("inl %1, %0" : "=a"(val) : "Nd"(port) );

    return val;
}

void annos::x86::out32(uint16_t port, uint32_t val)
{
    asm("outl %0, %1" : : "a"(val), "Nd"(port) );
}

void annos::x86::iodelay(unsigned n)
{
    volatile unsigned i = 0;
    for (i = 0; i < n; i++)
	asm volatile("xor %eax, %eax"); //32 bit NOP
	
}
