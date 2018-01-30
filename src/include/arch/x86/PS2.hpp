#pragma once

/*
  Driver for the 8042 PS/2 controller, the keyboard controller for the
  old days.

  It's marked as driver specific because you'll probably only find this
  chip on x86-class computers

  Copyright (C) 2018 Arthur M
*/

#include <KeyboardDevice.hpp>
#include <arch/x86/IRQHandler.hpp>
#include <arch/x86/IO.hpp>

namespace annos::x86 {

    // Maximum bytes for the keyboard queue
    #define MAX_KBD_QUEUE 64

    class PS2 : public KeyboardDevice, public IIRQHandlerDevice {
    private:
	volatile struct {
	    unsigned read_cur = 0, write_cur = 0;
	    char queue[MAX_KBD_QUEUE];
	} kbd_queue;

	// Number of PS/2 channels in this machine
	// Maximum is 2 (keyboard and mouse)
	unsigned char max_channels = 1;

	// PS/2 controller registers
	const uint16_t DATA_PORT = 0x60;   // read data
	const uint16_t STATUS_REG = 0x64;  // read status
	const uint16_t COMMAND_REG = 0x64; // write commands

	/**
	 * Send a command to the device currently selected
	 * Return true on success, false on error
	 */
	bool SendCommand(uint8_t code, unsigned port = 1, int value = -1);

	/**
	 * Reset a device on port 'port'
	 *
	 * @return true on success, false on failure
	 */
	bool Reset(unsigned port);

	/**
	 * Initialize the keyboard
	 *
	 * @return true on success, false on failure
	 */
	bool InitKeyboard();
	
    protected:
	/**
	 * Gets a key from the device
	 * 
	 * @return true if there's a key pressed in the buffer, false if
	 * not. If true, fills key information in the 'KeyDevice' struct.
	 */
	virtual bool GetDeviceKey(KeyDevice& d){return false;}

	/**
	 * Turns the LED of the 'lock' keys on or off, depending on the state
	 * parameter
	 */
	virtual void TurnLED(KeyCode kc, bool state){}
	
    public:
	PS2()
	    : KeyboardDevice("8042", "8042 PS/2 keyboard controller")
	    {}
	    
	virtual bool Detect();
	virtual void Initialize();
	virtual void Reset();

	/* Called every IRQ */
	virtual void OnIRQ(IRQRegs* regs);
	
    };
}


