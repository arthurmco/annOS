#include <Log.hpp>
#include <libk/stdio.h>
#include <stdarg.h>

/*
  Subsystem for the kernel logger

  Copyright (C) 2018 Arthur M
*/

using namespace annos;

Console* Log::_cons = NULL;

static void PrintLogLevel(Console* c, LogLevel l)
{
    switch(l) {
    case Debug: c->WriteVGA("[DEBUG] "); break;
    case Notice: c->WriteVGA("[NOTICE] "); break;
    case Info: break;
    case Warning: c->WriteVGA("[WARNING] ", BaseColors::Yellow); break;
    case Error: c->WriteVGA("[ERROR] ", BaseColors::LightRed); break;
    case Fatal: c->WriteVGA("\033[41;37;1m[FATAL]\033[0m "); break;
    }
}

void Log::Init(Console* c)
{
    Log::_cons = c;
}

void Log::Write(LogLevel l, const char* tag, const char* fmt, ...)
{
    PrintLogLevel(Log::_cons, l);

    char fstr[strlen(tag) + strlen(fmt) + 256];
    
    va_list vl;
    va_start(vl, fmt);
    vsprintf(fstr, fmt, vl);
    va_end(vl);

    char str[strlen(fstr) + strlen(tag) + 8] = "\033[1m";
    strcat(str, tag);
    strcat(str, "\033[0m: ");
    strcat(str, fstr);
    strcat(str, "\n");

    Log::_cons->WriteVGA(str);

    if (l >= LogLevel::Fatal)
	kputs("A fatal error occurred: \n"); // You should panic on fatals
    else if (l >= LogLevel::Warning)
	kputs(str); // if > warning, print to the screen too
}

