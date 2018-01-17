/** IDT flush and stub functions
 *
 * They can't be coded in pure C because of specific instructions for
 * resume to the code before interrupt (in case of the stub IDT functions)
 * or because of specific instructions who are better coded in pure
 * assembly (the case of idt_flush)
 * 
 * Copyright (C) 2018 Arthur M
 */

.global idt_flush
.type idt_flush, @function
idt_flush:
/** uint32_t idt_flush(IDTPointer* ptr)
 * Registers the IDT into the processor
 * Returns the IDT pointer you passed.
 */

    pushl   %ebp
    movl    %esp, %ebp

 // Gets the IDT pointer
	
    movl    8(%ebp), %ecx

    lidt (%ecx)

    movl    %ebp, %esp
    popl    %ebp
    ret

