/**
 * The IRQ handler code for the x86 architecture
 *
 * IRQ means Interrupt Request, something that the device 
 * issues when it needs processor attention
 *
 * The interrupt vectors 32 to 48 are reserved for IRQs
 * This is what the drivers will use, so watch out...
 */

#include <arch/x86/IRQHandler.hpp>
#include <libk/stdio.h>
#include <libk/panic.h>

#include <Log.hpp>

using namespace annos::x86;

/**
 * Functions to IRQ handlers
 */
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();


IDT* IRQHandler::_idt;

// The IRQ controller being used
i8259* _irq_control;

void IRQHandler::Init(IDT* idt, i8259* irqcontrol)
{
    
    idt->Set(0x20, (uintptr_t)&irq0, 0x08);
    idt->Set(0x21, (uintptr_t)&irq1, 0x08);
    idt->Set(0x22, (uintptr_t)&irq2, 0x08);
    idt->Set(0x23, (uintptr_t)&irq3, 0x08);
    idt->Set(0x24, (uintptr_t)&irq4, 0x08);
    idt->Set(0x25, (uintptr_t)&irq5, 0x08);
    idt->Set(0x26, (uintptr_t)&irq6, 0x08);
    idt->Set(0x27, (uintptr_t)&irq7, 0x08);
    idt->Set(0x28, (uintptr_t)&irq8, 0x08);
    idt->Set(0x29, (uintptr_t)&irq9, 0x08);
    idt->Set(0x2A, (uintptr_t)&irq10, 0x08);
    idt->Set(0x2B, (uintptr_t)&irq11, 0x08);
    idt->Set(0x2C, (uintptr_t)&irq12, 0x08);
    idt->Set(0x2D, (uintptr_t)&irq13, 0x08);
    idt->Set(0x2E, (uintptr_t)&irq14, 0x08);
    idt->Set(0x2F, (uintptr_t)&irq15, 0x08);
    
    IRQHandler::_idt = idt;
    _irq_control = irqcontrol;
}

/* MAX_IRQ_HANDLERS (probably 16) handlers for 16 IRQs */
static fnIRQHandler irqHandlers[16][MAX_IRQ_HANDLERS] = {};

/**
 * IRQ dispatcher
 * Execute the fault handlers.
 */
extern "C" void IRQDispatcher(IRQRegs* regs)
{    
    kprintf("IRQ \033[1;36m%d\033[0m triggered\n", regs->irq_no);

    size_t pos = 0;
    while (irqHandlers[regs->irq_no][pos]) {
	irqHandlers[regs->irq_no][pos](regs);
	pos++;

	if (pos >= MAX_IRQ_HANDLERS)
	    break;
    }
	   

    _irq_control->SendEOI(regs->irq_no);
}

/*
 * Sets a new IRQ handler
 * @returns a index in the list of handlers
 */
int IRQHandler::SetHandler(unsigned irqno, fnIRQHandler handler)
{
    if (irqno >= 16)
	panic("the IRQHandler only supports IRQ numbers 0 to 15");


    // Get the IRQ count
    size_t len = 0;
    while (irqHandlers[irqno][len] != NULL) {
	len++;
	if (len == 1)
	    _irq_control->SetIRQMask(irqno, false); // We have a handler, unmask
	    
	if (len >= MAX_IRQ_HANDLERS)
	    break;
    }

    if (len >= 16) {
	// Handler for this IRQ is full
	panic("handler list is full");
	return -1;
    }

    irqHandlers[irqno][len] = handler;
    return int(len);
}

/*
 * Removes an IRQ handler
 */
void IRQHandler::RemoveHandler(unsigned irqno, int index)
{
    irqHandlers[irqno][index] = NULL;

    /* Check if we have no more handlers, so we can unmask it in the PIC */
    unsigned i = 0;
    while (irqHandlers[irqno][i] != NULL) {
	i++;
	if (i >= MAX_IRQ_HANDLERS)
	    break;
    }

    if (i >= MAX_IRQ_HANDLERS)
	_irq_control->SetIRQMask(irqno, true);
}

