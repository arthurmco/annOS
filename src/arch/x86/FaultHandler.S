/**
 * Fault handler assembly code
 *
 * They are specific IDT handlers for each one of the supported exceptions of
 * our kernel
 * TODO: Implement above the page fault handler
 *
 * Copyright (C) 2018 Arthur M
 */


.global fault0  // Division by zero
.global fault1  // Debug
.global fault2  // Non-maskable interrupt
.global fault3  // Breakpoint
.global fault4  // Overflow
.global fault5  // Bound range exceeded
.global fault6  // Invalid Opcode
.global fault7  // Device not available
.global fault8  // Double fault

.global fault10  // Invalid TSS
.global fault11  // Segment Not Present
.global fault12  // Stack segment fault
.global fault13  // GPF
.global fault14  // Page fault

/* Some exceptions push error codes to the stack, others don't push nothing
 * Those macros below nivelate that, so that the stack is uniform along
 * all handles
 */	
	
// Macro to insert an exception that doesn't push anything to the stack
.macro fault_nocode faultnum
    fault\faultnum\() :
    xchgw %bx, %bx
	
    push $0x0       //  dummy error code
    push $\faultnum   //  the exception number

    jmp _fault_asm_common //  jump to common exception code
.endm

//  macro to insert an exception that doesn't push something to the stack
//  (the double fault exception is a good example)
.macro fault_code faultnum
    fault\faultnum\() :
    push $\faultnum   //  the exception number

    jmp _fault_asm_common //  jump to common isr cdde
.endm

FAULT_NOCODE 0
FAULT_NOCODE 1
FAULT_NOCODE 2
FAULT_NOCODE 3
FAULT_NOCODE 4
FAULT_NOCODE 5
FAULT_NOCODE 6
FAULT_NOCODE 7
FAULT_CODE 8
FAULT_CODE 10
FAULT_CODE 11
FAULT_CODE 12
FAULT_CODE 13
FAULT_CODE 14
	
/**
 * Extern pointer to our fault dispatcher
 */
.extern FaultDispatcher

//  Interrupt pushes eip, cs, eflags, user esp and ss automatically
_fault_asm_common:
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
	
	movl $FaultDispatcher, %eax
	call *%eax  //  Bran say this preserves the EIP register
	
	//  Undo everything IN REVERSE ORDER
	pop %eax
	pop %gs
	pop %fs
	pop %es
		pop %ds
	popal
	add $8, %esp //  Clear the exception number and code
	iret	//  Return from executing code before the exception call.
		//  Restores CS, EIP, EFLAGS, SS and ESP.
	hlt // just in case
	

	

	
