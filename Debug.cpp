#include <stdio.h>
#include <stdarg.h>
#include "Chip8.h"
#include "Debug.h"

void Debug::Log(const char* format, ...)
{
#ifdef DEBUG
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);
    printf(buffer);
#endif
}