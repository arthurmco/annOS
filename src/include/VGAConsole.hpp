#pragma once

#include <Console.hpp>

namespace annos {
    class VGAConsole : public Console {
    private:
	// The location of the VGA framebuffer
	uint16_t* _framebuffer = (uint16_t*)0xB8000;
	uint8_t _xPos = 0, _yPos = 0;

	const int _width = 80;
	const int _height = 25;

	void WriteChar(const char c, BaseColors color);
	
    public:
	/* Write function for VGA-compatible output */
	virtual void WriteVGA(const char* str,
			      BaseColors color = BaseColors::LightGrey);
	
	/* Write function for RGB-compatible output */
	virtual void WriteRGB(const char* str,
			      uint8_t r, uint8_t g, uint8_t b);

	virtual void Clear();
    };
};
