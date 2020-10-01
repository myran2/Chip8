#pragma once

class Debug
{
public:
    static void Log(const char* text, ...);
private:
    static char prevMsg[256];
};