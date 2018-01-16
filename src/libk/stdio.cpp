#include <libk/stdio.h>
#include <libk/stdlib.h>

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



inline void _inttostr(uint64_t num, int base, char* retchar)
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
