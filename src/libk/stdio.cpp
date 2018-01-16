#include <libk/stdio.h>
#include <libk/stdlib.h>
#include <stdarg.h>

#define assert(expr) _assert(expr, __FILE__, __LINE__)

extern void _assert(int expr, const char* file, int line);

size_t strlen(const char* str)
{
    size_t i = 0;
    while (str[i] != '\0') {
	i++;
    }

    return i;
}

char* strrev(char* dest, char* src)
{
    size_t srclen = strlen(src);
    
    for (size_t i = 0; i < srclen; i++) {
	dest[i] = src[srclen-i-1];
    }
    dest[srclen] = '\0';

    return dest;
}

int strncmp(const char* str1, const char* str2, size_t len)
{
    return memcmp((const void*)str1, (const void*)str2, len);
}

char* strncpy(char* dest, const char* src, size_t len)
{
    return (char*)memcpy((void*)dest, (const void*)src, len);
}

/* Integer based potentiation */
uint32_t ipow(uint32_t base, uint32_t exp)
{
    if (exp == 0) return 1;
    if (exp == 1) return base;
    
    for (unsigned i = 1; i < exp; i++) {
	base *= base;
    }

    return base;
}



inline void _inttostr(uint64_t num, int base, char* retchar, int padding=0,
    char padchar=' ')
{
    static const char* table = "0123456789abcdefghijklmnopqrstuvwxyz";

    char str[64];
    char revstr[64];
        
    auto i = 0;
    while (num >= (unsigned)base) {
	auto idx = num % base;
	str[i++] = table[idx];
	num /= base;
    }
    str[i++] = table[num];
    str[i] = '\0';
    
    for (size_t p = i; p < padding; p++) {
	str[p] = padchar;

	if (p == (padding-1))
	    str[p+1] = '\0';
    }
    
    
    strrev(revstr, str);
    strncpy(retchar, revstr, strlen(str)+1);
}

inline uint64_t _strtoint(const char* str, int base)
{
    size_t len = strlen(str);

    uint64_t num = 0;
    auto n = 0;
    for (int i = int(len)-1; i >= 0; i--) {
 	char c = str[i];
	auto snum = 0;
	if (c >= '0' && c <= '9')
	    snum = (c - '0');

	if (c >= 'A' && c <= 'Z')
	    snum = (c - 'A') + 10;

	if (c >= 'a' && c <= 'z')
	    snum = (c - 'a') + 10;

	assert(num < base);
	
	num += (snum * ipow(base, n));
	n++;
    }

    return num;
}

int atoi(const char* str)
{
    return (int)_strtoint(str, 10);
}

long atol(const char* str)
{
    return (long)_strtoint(str, 10);
}

void itoa(int num, char* str)
{
    _inttostr((uint64_t)num, 10, str);
}

void vsprintf(char* str, const char* fmt, va_list vl) {
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            unsigned padding = 0;
            char padchar = ' ';
            char padstr[8];
            int padstrptr = 0;
            /* detect padding specifiers */
            while ( (fmt[padstrptr] >= '0' &&
                     fmt[padstrptr] <= '9') ) {
                    if (padstrptr == 0 && fmt[padstrptr] == '0')
                    {
                        padchar = '0';
                        fmt++;
                        continue;
                    }
                    padstr[padstrptr] = fmt[padstrptr];
                    padstrptr++;

            }

            if (padstrptr > 0) {
                padstr[padstrptr] = 0;
                padding = atoi(padstr);
                fmt += padstrptr;
            }

            //tokens
            switch (*fmt) {
                //literal %
                case '%':
                    *str = '%';
                    break;

                //signed integer
                case 'd': {
                    char num[16];
                    int32_t i = va_arg(vl, int32_t);
                    if (padding > 0)
			_inttostr(i, 10, num, padding, padchar);
                    else
			_inttostr(i, 10, num);
                    *str = 0;
                    str = strcat(str, num);
                    str++;

                }
                    break;

                    //unsigned integer
                case 'u': {
                    char num[16];
                    int32_t i = va_arg(vl, int32_t);
                    if (padding > 0)
			_inttostr((i & 0x7fffffff), 10, num, padding, padchar);
                    else
                        _inttostr((i & 0x7fffffff), 10, num);
                    *str = 0;
		    if (i < 0)
			str = strcat(str, "-");
		    
                    str = strcat(str, num);
                    str++;

                }
                break;

                //hex
                case 'x': {
                    char num[8];
                    uint32_t i = va_arg(vl, uint32_t);
                    if (padding > 0)
			_inttostr(i, 16, num, padding, padchar);
                    else
                        _inttostr(i, 16, num);
                    *str = 0;
                    str = strcat(str, num);
                    str++;
                }
                    break;

                //string
                case 's': {
                    char* s = va_arg(vl, char*);
                    *str = 0;
                    str = strcat(str, s);
                    int truepad = padding-strlen(s);
                    if (truepad > 0) {
                        for (int i = 0; i < truepad; i++) {
                            str++;
                            *str = ' ';

                        }
                        str++;
                        *str = 0;
                        str--;
                    }
                    str++;

                }
                    break;

                //char
                case 'c': {
                    char c = va_arg(vl, int);
                    str++;
                    *str = c;
                }
                    break;
            }

        } else {
            *str = *fmt;
            str++;
        }
        fmt++;
    }
    *str++ = 0;
    *str = 0;
}

char* strcat(char* dst, const char* str)
{
    size_t len = strlen(dst);
    char* cstart = &dst[len];

    while (*str) {
        *cstart = *str;
        cstart++;
        str++;
    }

    *cstart = 0;

    return --cstart;
}
