#pragma once

#include <Console.hpp>

/*
  Subsystem for the kernel logger

  Copyright (C) 2018 Arthur M
*/

namespace annos {

    enum LogLevel {
	Debug,   // For what you'd use a printf()
	Notice, 
	Info,    // Things you want to show that happened
	Warning,
	Error,
	Fatal,
    };

    class Log {
    private:
	static Console* _cons;
    public:
	static void Init(Console* c);

	static void Write(LogLevel l, const char* tag, const char* fmt, ...);

    };    
}
