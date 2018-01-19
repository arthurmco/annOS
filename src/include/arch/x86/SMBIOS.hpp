#pragma once

/*
  SMBIOS device class

  Copyright (C) 2018 Arthur M

 */

#include <Device.hpp>
#include <stdint.h>
#include <stddef.h>

namespace annos::x86 {

    /**
     * SMBIOS entry point table
     */
    struct SMBiosEntry {
	char anchor[4]; //The _SM_ magic string

	/**
	 * The checksum.
	 * This value, when added to every byte in this structure, using 8 bit
	 * addition, need to result in 0x0.
	 * Every byte means everything from offset 0x0 to 'length'
	 */
	uint8_t checksum; // The checksum,

	/* The structure length */
	uint8_t length;

	/* SMBIOS major and minor version for this structure */
	uint8_t major, minor;

	/* Size of the largest SMBIOS struct, with its formatted area
	   and text strings */
	uint16_t max_structure_size;

	/* Entry point revision 
	   Identified the formatting of the bits in 'entry_point_fmtd_area'

	   This value being 0x0 means that the EP is based on SMBIOS 2.1
	   Others are reserved
	 */
	uint8_t entry_point_revision;

	/* Entry point formatted area */
	uint8_t entry_point_fmtd_area[5];

	/* Intermediate anchor string.
	   Contains the string "_DMI_"
	   It's 16-byte aligned to allow legacy DMI browsers to find this 
	   entry point */
	uint8_t interm_anchor[5];

	uint8_t interm_checksum;

	/* Total length, in bytes, and physical address of the 
	   SMBIOS structure table 

	   The SMBIOS Structure Table is a structure containing all SMBIOS
	   structures packed together.
	 */
	uint16_t smbios_struct_len;
	uint32_t smbios_struct_addr;

	/* Number of structures present in the SMBIOS Structure Table */
	uint16_t smbios_struct_count;

	/* BCD coded revision of the SMBIOS */
	uint8_t smbios_bcd_revision;
	
    } __attribute__((packed));

    /**
     * SMBIOS structure header 
     */
    struct SMBiosStrHeader {
	/**
	 * The structure type. 
	 * 0-0x7f are spec-defined, others are OEM-defined
	 */
	uint8_t type;
	
	uint8_t length;   // The size, in bytes, of this structure, without text
	                  // strings
	uint16_t handle;  // The handle, for the Get Information SMBIOS function
    } __attribute__((packed));

    /**
     * SMBIOS bios information, type 0x0
     */
    struct SMBios_BiosInfo {
	SMBiosStrHeader hdr;
	uint8_t vendor_strptr;
	uint8_t version_strptr;
	uint16_t starting_segment;
	uint8_t releasedate_strptr;

	// Size, where 64k * (n+1) is the BIOS ROM size
	uint8_t rom_size;

	uint64_t bios_characteristics;
    }  __attribute__((packed));

    /**
     * SMBIOS system information, type 0x1
     */
    struct SMBios_SysInfo {
	SMBiosStrHeader hdr;
	uint8_t manufacturer_sp;
	uint8_t prodname_sp;
	uint8_t version_sp;
	uint8_t serialnumber_sp;
	uint8_t uuid[16];
	uint8_t wakeup_event;
	uint8_t sku_number_sp;
	uint8_t model_family_sp;
    } __attribute__((packed));

    /**
     * SMBIOS baseboard/module information, type 0x2
     * Includes motherboards, planars, server blades or other modules
     */
    struct SMBios_Baseboard {
	SMBiosStrHeader hdr;
	uint8_t manufacturer_sp;
	uint8_t prodname_sp;
	uint8_t version_sp;
	uint8_t serialnumber_sp;
	uint8_t asset_tag_sp;
	uint8_t feature_flags;	
	uint8_t chassis_location_sp;
	uint16_t chassis_handle;
	uint8_t board_type;
    } __attribute__((packed));

    /**
     * SMBIOS processor information, type 0x4
     * Useful for getting accurate clock speeds
     */
    struct SMBios_Processor {
	SMBiosStrHeader hdr;
	uint8_t socket_sp;
	uint8_t proc_type;
	uint8_t proc_family;
	uint8_t proc_manufacturer_sp;
	uint64_t proc_id;
	uint8_t proc_version_sp;
	uint8_t voltage;
	uint16_t clock_mhz;
	uint16_t max_speed_mhz;
	uint16_t curr_speed_mhz;
	uint8_t status;
	// TODO: Implement remaining...
    } __attribute__((packed));

    
    /**
     * SMBIOS isn't really a device, is just a standard defining a table
     * in memory with component informations.
     *
     * The SMBIOS is the base for the dmidecode(8) command on linux, FYI
     */
    class SMBios : public Device {
    private:
	uintptr_t _smbios_entry_addr = NULL;

	/* Gets the string index 'idx' after the smbios table 'tbl'
	 */
	const char* GetSMBiosString(SMBiosStrHeader* hdr, unsigned idx);

	/* Parse SMBIOS BIOS information header */
	void ParseBiosInformation(SMBiosStrHeader* hdr);

	/* Parse SMBIOS system information header */
	void ParseSysInformation(SMBiosStrHeader* hdr);

	/* Parse SMBIOS baseboard information header */
	void ParseBoardInformation(SMBiosStrHeader* hdr);

	
	/* Parse SMBIOS processor header */
	void ParseSysProcessor(SMBiosStrHeader* hdr);
	
    public:
	SMBios()
	    : Device("smbios", "SMBIOS")
	    {}
	      

	/**
	 * Device detection code 
	 * Only detects if it's there, do not initialize it (or deinitialize
	 * right after you detect, even if you found it)
	 *
	 * @returns true if detected, false if not
	 */
	virtual bool Detect() override;
	
	virtual void Initialize();

	// SMBIOS doesn't need to be reset
	virtual void Reset(){}
	
    };
}

