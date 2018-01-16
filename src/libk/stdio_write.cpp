/**
   Any of the stdios that write something to the screen
**/

#include <libk/stdio.h>
#include <stdarg.h>

static annos::Console* cons = NULL;

void init_stdio(annos::Console* c)
{
    cons = c;
}

void kputs(const char* s)
{
    cons->WriteVGA(s);
}

void kprintf(const char* format, ...)
{
    char str[256+strlen(format)];
    va_list vl;
    va_start(vl, format);
    vsprintf(str, format, vl);
    va_end(vl);
    kputs(str);
}

void kputc(char c)
{
    char ch[2] = {c, '\0'};
    cons->WriteVGA(ch);
}

