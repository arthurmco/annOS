#pragma once

/**
 * Kernel libc stdio implementation
 * 
 * Copyright (C) 2018 Arthur M
 */

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <Console.hpp>

size_t strlen(const char* str);
int strncmp(const char* str1, const char* str2, size_t len);
char* strncpy(char* dest, const char* src, size_t len);

char* strrev(char* dest, char* src);

int atoi(const char* str);
long atol(const char* str);

void itoa(int num, char* str);

/* Integer based potentiation */
uint32_t ipow(uint32_t base, uint32_t exp);

void vsprintf(char* str, const char* fmt, va_list v);
char* strcat(char* dst, const char* str);

/* Initialize the stdio printing function */
void init_stdio(annos::Console* c);

void kputs(const char* s);
void kputc(char c);
void kprintf(const char* format, ...);
