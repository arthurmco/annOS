/*
	multiboot x86 bootloader for annos
 
 	Licensed under the MIT license
	Copyright 2018 Arthur M
 */

.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number to bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
	
.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.section .bss
// Allocate some 8kb stack space for the kernel

	.align 16
stack_bottom:
	.skip 8192
stack_top:

// Entry point
.section .text
.global _start
.type _start, @function
	
_start:
	mov $stack_top, %esp	// Sets the stack

// TODO: Initialize GDT
// TODO: Initialize IDT
// TODO: Initialize paging

	movl $0xb8004, %ebx
	movw $0x0f41, %cx
	movw %cx, (%ebx)

	call kernel_main
	
	cli
_end:
	hlt
	jmp _end
	
	
