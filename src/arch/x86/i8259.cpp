/**
 * Driver for the 8259 Programmable Interrupt Controller, an interrupt
 * controller chip present in the older PCs, and emulated on newer ones
 *
 * Copyright (C) 2018 Arthur M
 */

#include <arch/x86/i8259.hpp>
#include <arch/x86/IO.hpp>
#include <libk/panic.h>

using namespace annos::x86;

struct PICPorts {
    unsigned command, data;
};

constexpr PICPorts MasterPIC = {.command = 0x20, .data = 0x21};
constexpr PICPorts SlavePIC = {.command = 0xa0, .data = 0xa1};

/** 
 *  Implement device initialization here 
 */
void i8259::Initialize()
{
    /* Program the PIC to interrupts 32-48 (0x20-0x30) 
       so it don't conflict with the processor exceptions
     */

    // Small io_wait
    auto io_wait_busy = [](){
	volatile unsigned i = 0;
	for (i = 0; i < 500; i += 2) {
	    i--;
	}
    };
    
    out8(MasterPIC.command, 0x11); // ICW1 - begin initialisation;
    io_wait_busy();

    out8(SlavePIC.command, 0x11);
    io_wait_busy();

    out8(MasterPIC.data, 0x20+0); // Set the vector offsets
    io_wait_busy();
    out8(SlavePIC.data, 0x20+8);
    io_wait_busy();

    /* The slave irq controller is connected to the master one at IRQ 2.
       Say it to the master controller:
    */
    out8(MasterPIC.data, 1 << 2); //you have a slave controller at irq 2
    io_wait_busy();
    out8(SlavePIC.data, 2); //you're connected to the master at irq 2
    io_wait_busy();

    /* Tell the controller to operate in 8086 mode */
    out8(MasterPIC.data, 1);
    io_wait_busy();
    out8(SlavePIC.data, 1);
    io_wait_busy();
}
    

/**
 * Implement device reset here. 
 */
void i8259::Reset()
{
    // Save and restore saved masks, between the (re)initialization.
    uint8_t pic1m, pic2m;
    pic1m = in8(MasterPIC.data);
    pic2m = in8(SlavePIC.data);
    
    this->Initialize();

    out8(MasterPIC.data, pic1m);
    out8(SlavePIC.data, pic2m);

}


/**
 * Set/clear IRQ mask for a certain IRQ
 */
void i8259::SetIRQMask(unsigned irqno, bool status)
{
    if (irqno >= 16)
	panic("the 8259 controller only support 15 IRQs");

    unsigned port = (irqno >= 8) ? SlavePIC.data : MasterPIC.data;
    unsigned irqline = irqno % 8;

    uint8_t val = in8(port);
    if (status)
	val |= (1 << irqline);
    else
	val &= ~(1 << irqline);
    
    out8(port, val);    
}

/**
 * Get IRQ mask status
 *
 * @returns true if unmasked, false if masked
 */
bool i8259::GetIRQMask(unsigned irqno)
{
    // TODO: implement
    return false;
}
	

/**
 * Send an End of Interrupt signal
 */
void i8259::SendEOI(unsigned irqno)
{
    if (irqno >= 8)
	out8(SlavePIC.command, 0x20);

    out8(MasterPIC.command, 0x20);
}
