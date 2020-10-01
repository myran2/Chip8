#include <iostream>
#include "Chip8.h"

int main(int argc, char** argv) 
{
    if (argc < 2)
    {
        printf("Please supply a chip8 rom. Correct syntax is:\n%s \"romname.rom\" [-tick tickRate]\n", argv[0]);
        return 1;
    }
    
    int tickrate = 500;
    if (argc >= 3)
    {
        if (argv[1] == "-tick")
        {
            tickrate = atoi(argv[2]);
            printf("-tick flag specified tickrate to %d\n", tickrate);
        }
    }
    
    Chip8 emu;
    int errorCode = emu.Init(tickrate);
    if (errorCode != 0)
    {
        printf("Failed to initialize Chip8 emulator. Error code: %d\n", errorCode);
        return 1;
    }
    printf("Initialized Chip8 emulator\n");



    errorCode = emu.LoadGame(argv[1]);
    if (errorCode != 0)
    {
        printf("Failed to load Chip8 rom %s. Error code: %d\n", "TEST_ROM", errorCode);
        return 1;
    }

    printf("Starting %s...\n", "TEST_ROM");
    emu.Run();

    return 0;
}