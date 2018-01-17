#pragma once

#include <arch/x86/IDT.hpp>
#include <arch/x86/i8259.hpp>

/**
 * The IRQ handler code for the x86 architecture
 *
 * IRQ means Interrupt Request, something that the device 
 * issues when it needs processor attention
 *
 * The interrupt vectors 32 to 48 are reserved for IRQs
 * This is what the drivers will use, so watch out...
 */

namespace annos::x86 {

    // Max of handlers per irq
#define MAX_IRQ_HANDLERS 16
    
    /**
     * Represents register layout within IRQs
     */
    struct IRQRegs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, old_esp, ebx, edx, ecx, eax;
	uint32_t irq_no;
	uint32_t eip, cs, eflags, esp, ss;
    };

    /**
     * Function pointer representing a IRQ handler
     */
    typedef void (*fnIRQHandler)(IRQRegs* regs);
    
    /**
     * The fault handler class itself
     */
    class IRQHandler {
    private:
	static IDT* _idt;
		
    public:
	
	/**
	 * Set the IDT handler for each one of the IRQs
	 */
	static void Init(IDT* idt, i8259* irqcontrol);

	/*
	 * Sets a new IRQ handler
	 * @returns a index in the list of handlers
	 */
	static int SetHandler(unsigned irqno, fnIRQHandler handler);

	/*
	 * Removes an IRQ handler
	 */
	static void RemoveHandler(unsigned irqno, int index);
    };

}
