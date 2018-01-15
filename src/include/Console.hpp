#pragma once

#include <stdint.h>

/*
  Subsystem for the kernel console driver

  Provides basic functions regarding console output.
*/

namespace annos {
    /* The VGA-mode base colors */
    enum BaseColors {
	Black = 0,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Brown,
	LightGrey,
	DarkGrey,
	LightBlue,
	LightGreen,
	LightCyan,
	LightRed,
	LightMagenta,
	Yellow,
	White = 0xf,
    };
    
    class Console {
    public:
	/* Write function for VGA-compatible output */
	virtual void WriteVGA(const char* str, BaseColors color) = 0;
	
	/* Write function for RGB-compatible output */
	virtual void WriteRGB(const char* str,
			      uint8_t r, uint8_t g, uint8_t b) = 0;

	
    };
}
