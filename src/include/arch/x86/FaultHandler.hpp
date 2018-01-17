#pragma once

#include <arch/x86/IDT.hpp>

/**
 * The fault handler code for the x86 architecture
 *
 * The interrupt vectors 0 to 31 are reserved for faults.
 */

namespace annos::x86 {

    /**
     * Most of error codes for each one of the exceptions that the
     * processor can raise
     */
    enum FaultCode {
	DivideByZero = 0,
	Debug,
	NMI,
	Breakpoint,
	Overflow,
	BoundRange,
	InvalidOpcode,
	DeviceNotAvailable,
	DoubleFault,
	RSVD1,  //used to be Coprocessor Segment Overrun, not used anymore
	InvalidTSS,
	SegmentNotPresent,
	StackSegmentFault,
	GPF,
	PageFault,
	RSVD2,
	FPE,
	AlignmentCheck,
	MachineCheck,
	SIMDFPE,
	// Others reserved or not implemented.
    };
    
    /**
     * Represents register layout within faults
     */
    struct FaultRegs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, old_esp, ebx, edx, ecx, eax;
	uint32_t int_no, error_code;
	uint32_t eip, cs, eflags, esp, ss;
    };

    /**
     * Function pointer representing a fault handler
     */
    typedef void (*fnFaultHandler)(FaultRegs* regs);
    
    /**
     * The fault handler class itself
     */
    class FaultHandler {
    private:
	static IDT* _idt;

    public:
	
	/**
	 * Set the handler for each one of the faults in the IDT
	 */
	static void Init(IDT* idt);

	static void SetHandler(unsigned faultno, void* handler);
    };

}
