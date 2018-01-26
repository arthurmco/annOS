#pragma once

/**
 * PCI bus driver
 *
 * Copyright (C) 2018 Arthur M
 */

#include <Device.hpp>

namespace annos {

    /* Format of the query for sending into CONFIG_ADDRESS
       When we send these, we can read/write CONFIG_DATA, and it will return/store
       the data from/in the register 'register'
    */
    union PCIAddressQuery {
	uint32_t data;
	struct {
	    unsigned align:2;
	    unsigned reg:6;
	    unsigned function:3;
	    unsigned device:5;
	    unsigned bus:8;
	    unsigned rsvd:7;
	    unsigned enable:1;
	};
    };

    /* PCI device structure 
     * Created from info stored on PCI registers
     */
    struct PCIRegister {
	// PCI vendor and device ID
	uint16_t vendor, device;

	/* Command to send to PCI device */
	uint16_t command;

	/* PCI device status */
	uint16_t status;

	/* PCI device program ID, revision, class and subclass */
	uint8_t rev, prog_id;
	uint8_t subclass, classcode;

	uint8_t cache_line_size, latency_timer;
	uint8_t header_type, bist;

	/* From now, the header changes based on the header type
	   It can be a common device (type 0), a PCI to PCI bridge (type 1),
	   or a PCI to Cardbus bridge (type 2)
	*/
	union {
	    struct {
		uint32_t bar[6];
		uint32_t cardbus_cis_ptr;
		uint16_t subsys_vendor, subsys_id;
		uint32_t expansion_rom_base;
		uint8_t capabilities_ptr;
		uint8_t rsvd[7];
		uint8_t interrupt_line, interrupt_pin;
		uint8_t min_grant, max_latency;
	    } dev;
	    struct {
		uint32_t bar[2];
		uint8_t primary_bus_num, secundary_bus_num;
		uint8_t subordinate_bus_num, secondary_lat_timer;
		uint8_t iobase, iolimit;
		uint16_t secondary_status;
		uint16_t mem_base, mem_limit;
		uint16_t pref_mem_base, pref_mem_limit;
		uint32_t pref_base_upper, pref_limit_upper;
		uint8_t capabilities_ptr;
		uint8_t rsvd[3];
		uint32_t expansion_rom_base;
		uint8_t interrupt_line, interrupt_pin;
		uint16_t bridge_control;
	    } __attribute__((packed)) pci2pci;
	    struct {

	    } __attribute__((packed)) pci2cbus;
	} __attribute__((packed));
	
    } __attribute__((packed));

    /**
     * PCI Device information
     */
    struct PCIDev {
	uint8_t bus, dev, func;
	PCIRegister reginfo;
    };
    
    class PCIBus : public Device {
	friend class PCIDevice;
	
    private:	
	const unsigned short CONFIG_ADDRESS = 0xCF8;
	const unsigned short CONFIG_DATA = 0xCFC;

	/**
	 * Make a read with 'size' bytes in the PCI register 'idx' of device
	 * 'dev'
	 *
	 * @return the content read
	 *
	 * @remarks Note that 'size' can only be a multiple of 8
	 */
	template<uint8_t size>
	unsigned ReadPCIRegister(PCIDev* dev, unsigned idx);
	// TODO: Handle the case of register query crossing a 4-byte boundary
	// (i.e, you have to do 2 reads to get the full register)
	// No one of the PCI common registers needs this. Should I treat this
	// or this will never happen?
	
    public:
	PCIBus()
	    : Device("pcibus", "PCI bus device")
	    {}


	/** 
	 *  Initializate and discover the devices connected there
	 */
	virtual void Initialize();


	/**
	 * Do nothing
	 * TODO: _Re_discover the devices?
	 */
	virtual void Reset();

    };    
}
