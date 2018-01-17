/**
 * IRQ handler assembly code
 *
 * They are specific IDT handlers for each one of the supported IRQs of
 * our kernel
 * For now, they go from 0 to 15.
 *
 * Copyright (C) 2018 Arthur M
 */


.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
	
// Macro to insert a stub that jumps to our IRQ handler
.macro irq_macro irqno
irq\irqno\() :
    cli	
	
    push $\irqno   //  the exception number

    jmp _irq_asm_common //  jump to common exception code
.endm

IRQ_MACRO 0
IRQ_MACRO 1
IRQ_MACRO 2
IRQ_MACRO 3
IRQ_MACRO 4
IRQ_MACRO 5
IRQ_MACRO 6
IRQ_MACRO 7
IRQ_MACRO 8
IRQ_MACRO 9
IRQ_MACRO 10
IRQ_MACRO 11
IRQ_MACRO 12
IRQ_MACRO 13
IRQ_MACRO 14
IRQ_MACRO 15
	
/**
 * Extern pointer to our fault dispatcher
 */
.extern IRQDispatcher

//  Interrupt pushes eip, cs, eflags, user esp and ss automatically
_irq_asm_common:
	pushal  //  Push registers (edi,esi,ebp,esp.ebx,edx.ecx.eax)
	push %ds //  Push data segments
	push %es
	push %fs
	push %gs
	
	mov $0x10, %ax //  Load the kernel data segment descriptor
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	mov %esp, %eax //  We save the stack so we can restore it later
	push %eax
	
	movl $IRQDispatcher, %eax
	call *%eax  //  Bran say this preserves the EIP register
	
	//  Undo everything IN REVERSE ORDER
	pop %eax
	pop %gs
	pop %fs
	pop %es
	pop %ds
	
	popal
	add $4, %esp //  Clear the exception number
	iret	//  Return from executing code before the exception call.
		//  Restores CS, EIP, EFLAGS, SS and ESP.
	

	

	
