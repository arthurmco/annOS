#pragma once

/**
 * Kernel timer
 *
 * The timer is supposed to tick each milisecond, for each PIT on each 
 * architecture
 * It gives the kernel the notion of time.
 *
 * Copyright (C) 2018 Arthur M
 */

#include <stdint.h>

namespace annos {

    class Timer {
    private:
	static uint64_t v;
    public:
	static void Init();
	static void Tick();
	static uint64_t Get();
    };    
}
