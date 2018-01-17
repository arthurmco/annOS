#include <libk/panic.h>
#include <libk/stdio.h>

void _assert(int expr, const char* file, int line)
{
    if (!expr) {
	kprintf("\n\nAssertion failed at %s:%d. System halted\n",
		file, line);
	
	asm volatile("cli; hlt");
    }

}

void panic(const char* str)
{
    kprintf("\n\npanic: %s\n", str);
    asm volatile("cli; hlt");
}
