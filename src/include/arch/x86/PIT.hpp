#pragma once

/**
 * Driver for the x86 Programmable Interval Timer, the Intel 8253
 *
 * Copyright (C) 2018 Arthur M
 */

#include <Device.hpp>
#include <arch/x86/IRQHandler.hpp>
#include <Timer.hpp>
#include <stdint.h>

namespace annos::x86 {

    class PIT : public Device, public IIRQHandlerDevice {
    private:
	// Changes the new timer clock to 'hz'
	void SetTimer(uint16_t hz);
	
    public:
	PIT()
	    : Device("pit", "Intel 8253 compatible Programmable Interval Timer")
	    {}

	/** 
	 *  Device initialization
	 *  Sets the clock and the IRQ handler
	 */
	virtual void Initialize();

	/**
	 * Device reset.
	 * For now, just panic. We might not need to reset this.
	 */
	virtual void Reset();

	virtual void OnIRQ(IRQRegs* regs);
	    
    };
}
