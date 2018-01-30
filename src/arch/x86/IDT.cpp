#include <arch/x86/IDT.hpp>
#include <libk/panic.h>

using namespace annos::x86;

/** Set an entry to the IDT
 * @param intno The interrupt number
 * @param addr The address to jump on this interrupt
 * @param selector The GDT selector
 */
void IDT::Set(unsigned intno, uintptr_t addr, uint16_t selector,
    InterruptType type)
{
    if (intno >= 256) {
	panic("interrupt is bigger than 256");
    }

    this->_desc[intno].offset_low = (addr & 0xffff);
    this->_desc[intno].offset_high = (addr >> 16);
    this->_desc[intno].selector = selector;

    ; //Type code for interrupt, always present, DPL 0
    this->_desc[intno].type_attr = type | 0x80;
}

// Flushes the IDT into the processor
extern "C" uint32_t idt_flush(IDTPointer* ptr);

/** Register the IDT into the processor
 */
void IDT::Register()
{
    this->_ptr.addr = (uint32_t)&this->_desc[0];
    this->_ptr.size = sizeof(IDTDescriptor)*0x80-1;
    
    idt_flush(&this->_ptr);
}
