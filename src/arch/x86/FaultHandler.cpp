#include <arch/x86/FaultHandler.hpp>
#include <libk/stdio.h>

using namespace annos::x86;

/**
 * String representation for each exception
 */
static const char* exceptionStr[] = {
    "Division by Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "Floating Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Error"
};

/**
 * Functions to exception handlers
 */
extern "C" void fault0();     //Divide by zero
extern "C" void fault1();     //Debug
extern "C" void fault2();     //Non-maskable interrupts
extern "C" void fault3();     //Breakpoint
extern "C" void fault4();     //Overflow
extern "C" void fault5();     //Bound range exceeded
extern "C" void fault6();     //Invalid Opcode
extern "C" void fault7();     //Device not avaliable
extern "C" void fault8();     //Double fault

extern "C" void fault10();    //Invalid TSS
extern "C" void fault11();    //GDT segment not present
extern "C" void fault12();    //Stack segment fault
extern "C" void fault13();    //GPF
extern "C" void fault14();    //Page fault


/**
 * Fault dispatcher
 * Execute the fault handlers.
 */
extern "C" void FaultDispatcher(FaultRegs* regs)
{
    kprintf("\n\n");
    kprintf("Exception code: %d (%s)\nCode %04x", regs->int_no,
	    exceptionStr[regs->int_no], regs->error_code);
    
}

IDT* FaultHandler::_idt;

void FaultHandler::Init(IDT* idt)
{
    idt->Set(FaultCode::DivideByZero, (uintptr_t)&fault0, 0x08);
    idt->Set(FaultCode::Debug, (uintptr_t)&fault1, 0x08);
    idt->Set(FaultCode::NMI, (uintptr_t)&fault2, 0x08);
    idt->Set(FaultCode::Breakpoint, (uintptr_t)&fault3, 0x08, InterruptType::Trap);
    idt->Set(FaultCode::Overflow, (uintptr_t)&fault4, 0x08, InterruptType::Trap);
    idt->Set(FaultCode::BoundRange, (uintptr_t)&fault5, 0x08);
    idt->Set(FaultCode::InvalidOpcode, (uintptr_t)&fault6, 0x08);
    idt->Set(FaultCode::DeviceNotAvailable, (uintptr_t)&fault7, 0x08);
    idt->Set(FaultCode::DoubleFault, (uintptr_t)&fault8, 0x08);
    idt->Set(FaultCode::InvalidTSS, (uintptr_t)&fault10, 0x08);
    idt->Set(FaultCode::SegmentNotPresent, (uintptr_t)&fault11, 0x08);
    idt->Set(FaultCode::StackSegmentFault, (uintptr_t)&fault12, 0x08);
    idt->Set(FaultCode::GPF, (uintptr_t)&fault13, 0x08);
    idt->Set(FaultCode::PageFault, (uintptr_t)&fault14, 0x08);
    
    FaultHandler::_idt = idt;
}

void FaultHandler::SetHandler(unsigned faultno, void* handler)
{

}
