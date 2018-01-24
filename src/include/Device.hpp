#pragma once

/*
  Device representation class.

  Copyright (C) 2018 Arthur M

 */

#include <stdint.h>

namespace annos {
    class Device {
    protected:
	const char* _tag;
	const char* _name;

    public:
	/**
	 * Creates a device
	 * @param tag A short name for identifying the device. Can be the chip
	 *            name or something like that, like 'gt100' or 'i8052'.
	 * @param name A visible, human-readable name for the device, like
	 *             "Disk Drive Controller"
	 */
	Device(const char* tag, const char* name)
	    : _tag(tag), _name(name)
	    {}

	const char* GetTag();
	const char* GetName();

	/**
	 * Device detection code 
	 * Only detects if it's there, do not initialize it (or deinitialize
	 * right after you detect, even if you found it)
	 *
	 * @returns true if detected, false if not
	 */
	virtual bool Detect() { return true; }
	
	/** 
	 *  Implement device initialization here 
	 */
	virtual void Initialize() = 0;

	/**
	 * Implement device reset here. 
	 */
	virtual void Reset() = 0;

	/**
	 * Implement device ioctl process here 
	 * function_name is the function name, v0 and v1 are additional
	 * parameters, ret is the function result, ptr is a void pointer
	 * to additional args.
	 *
	 * Returns 0 for a successful ioctl, or -error for an error.
	 */
	int ProcessIoctl(uint32_t function_name,
			 uint64_t v0, uint64_t v1,
			 uint64_t* ret, void* ptr);

	
    };
}
