#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * Set up the Interrupt Descriptor Table
 *
 * The interrupt descriptor table, aka IDT, is a list of descriptors you give
 * to the processor, showing what address to execute for each interruption
 * that happens
 *
 * Copyright (C) 2018 Arthur M
 */
namespace annos::x86 {

    enum InterruptType {
	Interrupt = 0xE, // automatically disable interrupts on entry
	Trap = 0xF       // you need to disable interrupts
    };
    
    /** 
     * The pointer to the IDT.
     * The address of this is what you give to the processor 
     */
    struct IDTPointer {
	uint16_t size;
	uint32_t addr;
    } __attribute__((packed));

    /**
     * The descriptor itself
     * offset_low and offset_high are the low and high parts of the address 
     * the processor will execute for that interrupt
     *
     * selector is the selector of the GDT this interrupt will run.
     * More or less what mode.
     *
     * type is the type information
     */
    struct IDTDescriptor {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero = 0;
	uint8_t type_attr;
	uint16_t offset_high;
    } __attribute__((packed));
    
    class IDT {
    private:
	IDTPointer _ptr;
	IDTDescriptor _desc[256];
	
    public:

	/** Set an entry to the IDT
	 * @param intno The interrupt number
	 * @param addr The address to jump on this interrupt
	 * @param selector The GDT selector
	 */
	void Set(unsigned intno, uintptr_t addr, uint16_t selector,
		 InterruptType type = InterruptType::Interrupt);

	/** Register the IDT into the processor
	 */
	void Register();
    };
}
