
#include <Timer.hpp>
#include <libk/stdio.h>

/**
 * Kernel timer
 *
 * The timer is supposed to tick each milisecond, for each PIT on each 
 * architecture
 * It gives the kernel the notion of time.
 *
 * Copyright (C) 2018 Arthur M
 */

using namespace annos;

uint64_t Timer::v;

void Timer::Init()
{
    Timer::v = 0;
}

void Timer::Tick()
{
    Timer::v += 1;
    if (Timer::v % 1000 == 0)
	kprintf("%4d s\n", ((uint32_t)Timer::v/1000));
}

uint64_t Timer::Get()
{
    return Timer::v;
}
