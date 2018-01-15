#pragma once

#include <stdint.h>

/* 
   x86 I/O port communication file

   Copyright (C) 2018 Arthur M
 */

namespace annos::x86 {
    
    /* Communication functions with the I/O ports, with
       8, 16 or 32 bit port length variations
    */
    uint8_t in8(uint16_t port);
    void out8(uint16_t port, uint8_t);

    uint16_t in16(uint16_t port);
    void out16(uint16_t port, uint16_t);

    uint32_t in32(uint16_t port);
    void out32(uint16_t port, int32_t);
}
