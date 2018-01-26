#pragma once

/*
  Represents a device connected via PCI 

  Copyright (C) 2018 Arthur M
*/

#include <stddef.h>
#include <Device.hpp>
#include <PCIBus.hpp>

namespace annos {
    class PCIDevice : public Device {
    private:
	PCIBus* _bus;
	
    protected:
	/* PCI information about this device */
	PCIDev* pci = NULL;

	/*
	 * Check if PCI device 'class' with subclass 'subclass' is present
	 * This is useful for loading a driver that satisfies a class of devices,
	 * e.g, IDE controllers
	 * It fills the 'pci' member with the loaded info.
	 *
	 * @return true if detected, false if not
	 */
	bool DetectPCIByClass(uint16_t classcode, uint16_t subclass);


    public:
	PCIDevice(PCIBus* bus, const char* tag, const char* name)
	    : Device(tag, name),
	      _bus(bus)
	    {}

	virtual void Initialize() = 0;
	virtual void Reset() = 0;
    };
}
