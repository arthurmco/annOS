#include <DebugConsole.hpp>
#include <arch/x86/IO.hpp>

using namespace annos;
using namespace annos::x86;

extern void kputs(const char*);

/*
  Kernel serial logging console driver

  Outputs logging information to COM1 at 9600 baud, for ease retrieval of
  logger information

  TODO: When the *real* serial port driver starts, make this console driver
  use it, instead of its own thing

  Copyright (C) 2018 Arthur M
*/

void SerialLogConsole::WriteString(const char* str)
{
    while (*str != '\0') {
	this->WriteChar(*str);
	str++;
    }
}

/* Check if the line is empty before we transmit
   If is, then go
*/    
void SerialLogConsole::WriteChar(char c)
{
    volatile unsigned int timeout=0;
    while ( (in8(this->portno+5) & 0x20) == 0 ) {
	if ((timeout++) > 0xfffffff0) {
	    return;
	}
    }

    out8(this->portno, (uint8_t)c);
}
	

/* Do a basic setup of the serial ports */
SerialLogConsole::SerialLogConsole()
{
    out8(this->portno+1, 0); // no interrupts

    // 115200 Hz is the clock the UART runs
    const uint16_t divisor = 115200 / this->baud;
    
    // Open divisor latch, by setting the bit 7, to set the baud rate
    out8(this->portno+3, 0x80);
    out8(this->portno+0, divisor & 0xff);
    out8(this->portno+1, divisor >> 8);
    out8(this->portno+3, 0x3); // 8 bits, no parity, 1 stop bit (8N1)

    /* Clear the transmit and receive FIFOs, set FIFO threshold to 8 bytes
       and enable it
    */
    out8(this->portno+2, 0x87);
    this->WriteString("\n\n \033[1m-[annOS]-\033[0m \n");
    this->WriteString("Serial log console started: "
		      "\033[38;5;74m9600\033[0m baud, "
		      "\033[38;5;74m8n1\033[0m\n");
}
	
/* Write function for VGA-compatible output */
void SerialLogConsole::WriteVGA(const char* str, BaseColors color)
{
    // ANSI escape sequence for each one of the vga colors, in order
    const char* colorStr[] = {
	"\033[30m", "\033[34m", "\033[32m", "\033[36m",
	"\033[31m", "\033[35m", "\033[33m", "\033[37m",
	"\033[1;30m", "\033[1;34m", "\033[1;32m", "\033[1;36m",
	"\033[1;31m", "\033[1;35m", "\033[1;33m", "\033[1;37m"
    };
    
    if (color != BaseColors::LightGrey)
	this->WriteString(colorStr[color]);
    this->WriteString(str);
    if (color != BaseColors::LightGrey)
	this->WriteString("\033[0m");
}
	
/* Write function for RGB-compatible output */
void SerialLogConsole::WriteRGB(const char* str,
		      uint8_t r, uint8_t g, uint8_t b)
{
    this->WriteVGA(str);
}


/* Clears the screen */
void SerialLogConsole::Clear()
{
    this->WriteString("\033[2J");
}

