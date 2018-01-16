#pragma once

/**
 * Kernel libc panic (aka fatal error) functions
 * 
 * Copyright (C) 2018 Arthur M
 */

void _assert(int expr, const char* file, int line);
void panic(const char* str);

#define assert(expr) _assert(expr, __FILE__, __LINE__);
