/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB physical, a conventional place 
	   for kernels to be loaded at by the bootloader */
	   
	. = 0xc0100000;
	_p_kernel_start = .; /* Kernel physical start address */

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	   
	.text ALIGN(4K) : AT(ADDR (.text) - 0xc0000000)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR (.rodata) - 0xc0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR (.data) - 0xc0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) :  AT(ADDR (.bss) - 0xc0000000)
	{
		*(COMMON)
		*(.bss)
	}

	_p_kernel_end = .; /* Kernel physical end address */

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
