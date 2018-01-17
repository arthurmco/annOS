#include <VGAConsole.hpp>
#include <arch/x86/IO.hpp>
#include <libk/stdlib.h> //memcpy()

using namespace annos;

uint16_t* VGAConsole::_framebuffer = (uint16_t*)0xB8000;
static void update_cursor(uint16_t x, uint16_t y, uint16_t w)
{
    uint16_t pos = (y * w) + x;
    
    ::x86::out8(0x3d4, 0x0f);
    ::x86::out8(0x3d5, (pos & 0xff));
    ::x86::out8(0x3d4, 0x0e);
    ::x86::out8(0x3d5, (pos >> 8));    
}

// Scrolls the console up
void VGAConsole::Scroll()
{
    for (unsigned y = 1; y < _height; y++) {
	void* fbFrom = (void*) &this->_framebuffer[ (y*_width) ];
	void* fbTo = (void*) &this->_framebuffer[ ((y-1)*_width) ];
	
	memcpy(fbTo, fbFrom, _width * sizeof(uint16_t));
    }

    uint16_t* fbBottom = &this->_framebuffer[(_height-1)*_width];

    for (unsigned i = 0; i < this->_width; i++) {
	// Bottom is a black space char in white foreground
	fbBottom[i] = 0x0720;
    }
}

void VGAConsole::WriteChar(const char c, BaseColors fgcolor)
{
    if (c == '\n') {
	_xPos = 0;
	_yPos++;
    } else if (c == '\t') {
	_xPos += 4;
	_xPos = ((_xPos >> 2) << 2);
	if (_xPos > _width) {
	    _xPos = 0;
	    _yPos++;
	}
	
    } else {
	uint8_t color = (uint8_t)fgcolor;
	uint16_t data = (uint16_t)c | ((uint16_t)color << 8);
	_framebuffer[_yPos * _width + _xPos] = data;
	_xPos++;
    }

    if (_xPos >= _width) {
	_xPos = 0;
	_yPos++;
    }

    if (_yPos >= _height) {
	this->Scroll();
	_yPos = (_height-1);
    }
   
}

/* Write function for VGA-compatible output */
void VGAConsole::WriteVGA(const char* str, BaseColors color)
{
    while (*str != '\0') {
	this->WriteChar(*str, color);
	str++;
    }
    update_cursor(_xPos, _yPos, _width);
}

/* Clears the screen */
void VGAConsole::Clear()
{
    for (auto i = 0; i < _width*_height; i++) {
	// Clears it all with a black space char in white foreground
	_framebuffer[i] = 0x0720; 
    }

    _xPos = 0;
    _yPos = 0;
}

/* Write function for RGB-compatible output */
void VGAConsole::WriteRGB(const char* str, uint8_t r, uint8_t g, uint8_t b)
{
    
}
