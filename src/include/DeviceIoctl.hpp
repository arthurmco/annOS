#pragma once

/*
  List of common ioctl function numbers, used by the OS itself.

  Copyright (C) 2018 Arthur M

 */

#include <stdint.h>

namespace annos {
    /**
     * Ioctl function numbers 1 to 16384 are reserved by the OS.
     * Devices can use numbers above that.
     *
     */
    enum DeviceIoctl {
	/* Gets the device tag, returns it in 'ret' */
	DEVICE_GET_TAG = 1,

	/* Gets the device name, returns it in 'ret' */
	DEVICE_GET_NAME = 2,
    };

}
