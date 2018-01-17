#pragma once
/**
 * Driver for the 8259 Programmable Interrupt Controller, an interrupt
 * controller chip present in the older PCs, and emulated on newer ones
 *
 * Copyright (C) 2018 Arthur M
 */

#include <Device.hpp>

namespace annos::x86 {
    class i8259 : public Device {
    public:
	i8259()
	    : Device("i8259", "8259 Programmable Interrupt Controller")
	    {}

	/** 
	 *  Implement device initialization here 
	 */
	virtual void Initialize();

	/**
	 * Implement device reset here. 
	 */
	virtual void Reset();

	/**
	 * Enable/disable IRQ mask for a certain IRQ
	 */
	void SetIRQMask(unsigned irqno, bool status);

	/**
	 * Get IRQ mask status
	 *
	 * @returns true if unmasked, false if masked
	 */
	bool GetIRQMask(unsigned irqno);

	/**
	 * Send an End of Interrupt signal
	 */
	void SendEOI(unsigned irqno);
	
	/* Make no ioctl support for this device 
	 * This means no usermode access.
	 */
    };
}
