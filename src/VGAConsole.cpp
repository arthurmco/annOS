#include <VGAConsole.hpp>

using namespace annos;

void VGAConsole::WriteChar(const char c, BaseColors fgcolor)
{
    if (c == '\n') {
	_xPos = 0;
	_yPos++;
	return;
    }
    
    uint8_t color = (uint8_t)fgcolor;
    uint16_t data = (uint16_t)c | ((uint16_t)color << 8);
    _framebuffer[_yPos * _width + _xPos] = data;

    _xPos++;
    if (_xPos >= _width) {
	_xPos = 0;
	_yPos++;
    }
    
}

/* Write function for VGA-compatible output */
void VGAConsole::WriteVGA(const char* str, BaseColors color)
{
    while (*str != '\0') {
	this->WriteChar(*str, color);
	str++;
    }
}

/* Clears the screen */
void VGAConsole::Clear()
{
    for (auto i = 0; i < _width*_height; i++) {
	_framebuffer[i] = 0x20; // Clears it all with a black space char
    }

    _xPos = 0;
    _yPos = 0;
}

/* Write function for RGB-compatible output */
void VGAConsole::WriteRGB(const char* str, uint8_t r, uint8_t g, uint8_t b)
{
    
}
