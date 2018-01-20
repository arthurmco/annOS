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
	.skip 32768
stack_top:

/** The kernel_main() function (for now) expects that we pass a pointer to a
 *  struct. The struct will be declared where, along with its fields
 */
kboot_struct:
multiboot_magic:
	.skip 4 // The multiboot load magic number (0x2badb002)
multiboot_ptr:
	.skip 4 // Physical memory pointer to the multiboot boot info struct.
phys_pos_kernel_start:
	.skip 4 // Physical address of kernel start
phys_pos_kernel_end:	
	.skip 4 // Physical address of kernel end

// Fields below are not implemented
	
phys_virt_offset:
	.skip 4 // Offset of virtual address relative to the physical
	        // will be 0xC000000

// Pointers to kernel start and end
.extern _p_kernel_start
.extern _p_kernel_end
	
// Entry point
.section .text
.global _start
.type _start, @function
	
_start:
	mov $stack_top, %esp	// Sets the stack

	push %ebx
	mov $kboot_struct, %ebx // use ebx for addressing, it's made for this
	pop %ecx
	
	mov %eax, (%ebx) // put the magic number
	mov %ecx, 4(%ebx) // put the multiboot struct physical pointer
	mov $_p_kernel_start, %eax
	mov %eax, 8(%ebx)
	mov $_p_kernel_end, %eax
	mov %eax, 12(%ebx)

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
	
// TODO: Initialize paging

main_init:	
	movl $0xb8004, %ebx
	movw $0x0f41, %cx
	movw %cx, (%ebx)

	mov $kboot_struct, %ebx // load the pointer.
	push %ebx
	
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
