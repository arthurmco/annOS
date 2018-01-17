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
	.skip 16384
stack_top:

// Entry point
.section .text
.global _start
.type _start, @function
	
_start:
	mov $stack_top, %esp	// Sets the stack

gdt_init:
//Initialize GDT
	mov $gdt_descriptor, %ebx
	lgdt (%ebx)
	jmp $0x08, $.gdt_reload
	
	.gdt_reload:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	jmp main_init
	
// TODO: Initialize IDT
// TODO: Initialize paging

main_init:	
	movl $0xb8004, %ebx
	movw $0x0f41, %cx
	movw %cx, (%ebx)

	call kernel_main
	
	cli
_end:
	hlt
	jmp _end

.align 16
gdt_descriptor:
	.word (gdt_tables_end - gdt_tables) - 1
	.long gdt_tables

.align 16	
gdt_tables:
	
	// 0: the null table
	.long 0
	.long 0

	// 1: the kernel code descriptior
	.long 0x0000ffff
	.long 0x00CF9A00

	// 2: the kernel data description
	.long 0x0000ffff
	.long 0x00CF9200		
gdt_tables_end:	
	
	// 3: the usermode code description
	.long 0x0000ffff
	
	// 4: the usermode data description
	.long 0x0000ffff	
