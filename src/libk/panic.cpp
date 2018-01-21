#include <libk/panic.h>
#include <libk/stdio.h>

void _assert(int expr, const char* file, int line)
{
    if (!expr) {
	kprintf("\n\n\033[41;37;1mAssertion failed at %s:%d.\033[0m System halted\n",
		file, line);
	
	asm volatile("cli; hlt");
    }

}

void panic(const char* str)
{
    kprintf("\n\033[41;37;1mpanic:\033[0m %s\n", str);
    asm volatile("cli; hlt");
}
