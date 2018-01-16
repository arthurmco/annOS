#pragma once

/**
 * Kernel libc stdlib implementation
 * 
 * Copyright (C) 2018 Arthur M
 */

#include <stddef.h>

/**
 * Compares 'len' bytes of s1 and s2 and see if they are equal
 * 
 * @returns Returns zero if they're equals, -1 if s1 < s2 or 1 if s1 > s2
 */
int memcmp(const void* s1, const void* s2, size_t len);

/**
 * Copies 'len' bytes of 'src' to 'dest'
 *
 * @returns a pointer to dest
 */
void* memcpy(void* dest, const void* src, size_t len);

/**
 * Fills the first 'n' bytes of 's' with byte 'c'
 *
 * @returns a pointer to 's'
 */
void* memset(void* s, unsigned char c, size_t n);


