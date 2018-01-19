#include <libk/stdlib.h>


/**
 * Compares 'len' bytes of s1 and s2 and see if they are equal
 * 
 * @returns Returns zero if they're equals, -1 if s1 < s2 or 1 if s1 > s2
 */
int memcmp(const void* s1, const void* s2, size_t len)
{
    const char* cs1 = (const char*)s1;
    const char* cs2 = (const char*)s2;
    for (size_t i = 0; i < len; i++) {
	if (cs1[i] != cs2[i]) {
	    if (cs1[i] < cs2[i]) return -1;
	    if (cs1[i] > cs2[i]) return 1;
	}
    }

    return 0;
}

/**
 * Copies 'len' bytes of 'src' to 'dest'
 *
 * @returns a pointer to dest
 */
void* memcpy(void* dest, const void* src, size_t len)
{
    char* cdest = (char*)dest;
    const char* csrc = (const char*)src;
    for (auto i = 0; i < len; i++)
	cdest[i] = csrc[i];

    return dest;
}

/**
 * Fills the first 'n' bytes of 's' with byte 'c'
 *
 * @returns a pointer to 's'
 */
void* memset(void* s, unsigned char c, size_t n)
{
    char* cs = (char*)s;
    for (size_t i = 0; i < n; i++)
	cs[i] = c;

    return s;
}
