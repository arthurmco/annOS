#pragma once

#include <Console.hpp>

/*
  Kernel serial logging console driver

  Outputs logging information to COM1 at 9600 baud, for ease retrieval of
  logger information

  TODO: When the *real* serial port driver starts, make this console driver
  use it, instead of its own thing

  Copyright (C) 2018 Arthur M
*/

namespace annos {
    class SerialLogConsole : public Console {

    private:
	void WriteString(const char* str);
	void WriteChar(char c);

	const int portno = 0x3f8;
	const int baud = 115200;
	
    public:
	/* Do a basic setup */
	SerialLogConsole();
	
	/* Write function for VGA-compatible output */
	virtual void WriteVGA(const char* str,
			      BaseColors color = BaseColors::LightGrey);
	
	/* Write function for RGB-compatible output */
	virtual void WriteRGB(const char* str,
			      uint8_t r, uint8_t g, uint8_t b);


	/* Clears the screen */
	virtual void Clear();


    };
};

