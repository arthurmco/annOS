#pragma once

/**
 * Kernel libc stdio implementation
 * 
 * Copyright (C) 2018 Arthur M
 */

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* str);
int strncmp(const char* str1, const char* str2, size_t len);
char* strncpy(char* dest, const char* src, size_t len);

char* strrev(char* dest, char* src);

int atoi(const char* str);
long atol(const char* str);

void itoa(int num, char* str);

/* Integer based potentiation */
uint32_t ipow(uint32_t base, uint32_t exp);
