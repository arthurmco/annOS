#include <arch/x86/PS2.hpp>
#include <Log.hpp>

/*
  Driver for the 8042 PS/2 controller, the keyboard controller for the
  old days.

  It's marked as driver specific because you'll probably only find this
  chip on x86-class computers

  Copyright (C) 2018 Arthur M
*/


using namespace annos;
using namespace annos::x86;


bool PS2::Detect()
{
    return true; // We'll need ACPI to do it right
}

/**
 * Send a command to the device at port 'port'
 * It might be the device in the first channel or in the second
 * Return true on success, false on error
 */
bool PS2::SendCommand(uint8_t code, unsigned port, int value)
{
    // Flushes results from other commands
    while ((in8(STATUS_REG) & 0x1)) {in8(DATA_PORT); }
    
    if (port == 2) {
	out8(COMMAND_REG, 0xd4); // Write next byte to second PS/2 port
	iodelay(100); // with a short delay for the controller to process
	              // the command
    }
    
    out8(DATA_PORT, code);
    iodelay(100);

    if (value > 0) {
	out8(DATA_PORT, (uint8_t)value);
    }
    
    unsigned timeout = 0;
    while (!(in8(STATUS_REG) & 0x1)) {
	if (timeout == 20) {
	    Log::Write(Error, "ps2", "Timeout while sending command %02x to port %02x",
	       code, port);
	    return false;
	}

	timeout++;
	iodelay(10000*timeout);
	    
    }
    
    iodelay(50000);
    auto res = in8(DATA_PORT);
    Log::Write(Debug, "ps2", "Sent %02x, received %02x on port %02x",
	       code, res, port);

    // If device returned AFK (0xFA) or 0xAA, return true
    if (res == 0xAA || res == 0xFA)
	return true;

    Log::Write(Error, "ps2", "Failed to send command %02x to port %02x, returned %02x",
	       code, port, res);
    return false;
}

/**
 * Reset a device on port 'port'
 *
 * Return true on success, false on failure
 */
bool PS2::Reset(unsigned port)
{
    // Flushes results from other commands
    while ((in8(STATUS_REG) & 0x1)) {in8(DATA_PORT); }
    
    if (port == 2) {
	out8(COMMAND_REG, 0xd4); // Write next byte to second PS/2 port
	iodelay(100); // with a short delay for the controller to process
	              // the command
    }
    
    out8(DATA_PORT, 0xff);
    iodelay(50000);
    auto res = in8(DATA_PORT);

    // first 0xAA, then 0xFA, or vice-versa
    if (res != 0xAA && res != 0xFA) {
	Log::Write(Error, "ps2", "Failed to reset %02x #1, returned %02x",
		   port, res);
	return false;
    }

    iodelay(1000);
    if (in8(STATUS_REG) & 0x1) {
	res = in8(DATA_PORT);
	if (res != 0xAA && res != 0xFA) {
	    Log::Write(Error, "ps2", "Failed to reset %02x #2, returned %02x",
		       port, res);
	    return false;
	}
    }

    iodelay(1000);
    // A 0x0 might come
    if (in8(STATUS_REG) & 0x1) {
	res = in8(DATA_PORT);
	if (res != 0x0) {
	    Log::Write(Error, "ps2", "Failed to reset %02x #3, returned %02x",
		   port, res);
	    return false;
	}
    }

    return true;
}



void PS2::Initialize()
{
    /* BIOS already initialised the device, but not the way we like 
       We should reset it.
     */
    
    // 1 - Disable the two PS/2 ports (keyboard and mouse)
    out8(COMMAND_REG, 0xAD);
    iodelay(5000);
    out8(COMMAND_REG, 0xA7);
    iodelay(5000);

    // 2 - Flush the output buffer
    // 16 reads should be sufficient to clear any crap in the buffer
    for (unsigned i = 0; i < 16; i++)
	in8(DATA_PORT);

    Log::Write(Info, "ps2", "Buffers cleared");
    
    // 3 - Disable IRQs and translation
    // (so it can't bother us while we initialise)
    out8(COMMAND_REG, 0x20);

    // Read the command configuration byte, an area in the controller RAM
    // with some of its configurations.
    
    auto ccb = in8(DATA_PORT);

    // Here we can check the number of channels of this device
    this->max_channels = 1;
    
    if (ccb & (1 << 4))
	this->max_channels = 1; // first port is present

    if (ccb & (1 << 5))
	this->max_channels = 2; // second port is present

    Log::Write(Info, "ps2", "%d channels detected",
	       (unsigned)this->max_channels);

    out8(COMMAND_REG, 0x60);

    // Disable interrupts for both ports and translation
    ccb &= ~(0x1 | 0x2 | 0x40);

    out8(DATA_PORT, ccb);
    iodelay(50000);
    
    // 4 - Make the controller do a self test
    Log::Write(Debug, "ps2", "Controller self-test started");

    out8(COMMAND_REG, 0xAA);
    auto st_res = in8(DATA_PORT);
    Log::Write(Debug, "ps2", "Controller self-test result: 0x%02x", st_res);

    if (st_res != 0x55) {
	Log::Write(Error, "ps2", "Controller self-test failed with %02x",
		   st_res);
	//return false
    }

    // 5 - Test the channels themselves
    out8(COMMAND_REG, 0xAB); // Test the first port
    auto res = in8(DATA_PORT);
    if (res != 0x0) {
	Log::Write(Error, "ps2", "First port test failed with %02x",
		   res);
	//return false
    }

    if (this->max_channels >= 2) {
	out8(COMMAND_REG, 0xA9); // Test the second port
	res = in8(DATA_PORT);
	if (res != 0x0) {
	    Log::Write(Error, "ps2", "Second port test failed with %02x",
		       res);
	    //return false
	}
    }

    // 6 - Reset the devices
    this->Reset(1);
    if (this->max_channels >= 2)
	this->Reset(2);
    

    // 7 - Identify the devices
    // On the first
    this->SendCommand(0xf5, 1); // Disable scanning
    this->SendCommand(0xf2, 1); // Identify

    uint16_t devtype = 0;
    devtype = in8(DATA_PORT);
    if (devtype > 0x80)
	devtype |= (in8(DATA_PORT) << 8);

    Log::Write(Info, "ps2", "First port device type: %04x", devtype);

    // And on the second
    this->SendCommand(0xf5, 2); // Disable scanning
    this->SendCommand(0xf2, 2); // Identify
    
    devtype = 0;
    devtype = in8(DATA_PORT);
    if (devtype > 0x80)
	devtype |= (in8(DATA_PORT) << 8);

    Log::Write(Info, "ps2", "Second port device type: %04x", devtype);

    this->InitKeyboard();

    // Enable scanning for mouse
    this->SendCommand(0xf4, 2); // enable scanning, keyboard will send scancodes

    // Read the Controller Config Byte
    out8(COMMAND_REG, 0x20);
    iodelay(100);
    ccb = in8(DATA_PORT);

    out8(COMMAND_REG, 0x60);

    // Enable interrupts for both ports, keep translation disabled
    ccb |= 0x3;

    out8(DATA_PORT, ccb);
    
    // Enable both ports
    out8(COMMAND_REG, 0xAE); // the first;
    iodelay(1000);
    if (this->max_channels == 2)
	out8(COMMAND_REG, 0xA8);
}


/**
 * Initialize the keyboard
 *
 * @return true on success, false on failure
 */
bool PS2::InitKeyboard()
{
    Log::Write(Info, "ps2", "Initializing keyboard");

    this->SendCommand(0xf0, 1, 0x02); // set to scancode set 2
    this->SendCommand(0xf4, 1); // enable scanning, keyboard will send scancodes
}

void PS2::Reset()
{
    this->Initialize();
}


/* Called every IRQ */
void PS2::OnIRQ(IRQRegs* regs)
{
    if (regs->irq_no == 1) {
	uint8_t key = in8(DATA_PORT);
	Log::Write(Info, "ps2", "Data received: %02x", key);
    } else if (regs->irq_no == 12) {
	uint16_t mousek = in8(DATA_PORT);
	mousek |= (in8(DATA_PORT) << 8);
	
	Log::Write(Info, "ps2", "Mouse IRQ received %04x", mousek);
    }
    
}
