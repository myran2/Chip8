#pragma once
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <unordered_map>
#include <functional>
#include <SDL.h>

//#define DEBUG

#define FONT_START_ADDR 0x050
#define FONT_END_ADDR 0x0A0
#define FIRST_MEMORY_LOCATION 0x200


class Chip8
{
public:
    Chip8();
    ~Chip8();

    // initializes chip8 registers and memory for first run
    // returns 0 if no errors. Otherwise returns an error code.
    int Init(int tickrate);

    // loads game at specified location into chip8 memory.
    // returns 0 if no errors. Otherwise returns an error code.
    int LoadGame(const std::string& fileName);

    // starts the chip8 emulation cycle
    void Run();

    // emulates 1 cpu cycle
    void Tick();

    void SetProgramCounter(uint16_t pc);
    uint16_t GetProgramCounter() { return m_PC; }

    uint8_t GetRegister(uint8_t regIndex) const;
    void SetRegister(uint8_t regIndex, uint8_t val);

    uint16_t GetIndex() const;
    void SetIndex(uint16_t val);

    uint8_t GetMemory(uint16_t memIndex) const;
    void SetMemory(uint16_t memIndex, uint8_t val);

    void XorPixel(uint16_t pixel);
    bool GetPixelStatus(uint16_t pixel) const;

    bool GetDrawFlag() { return m_draw; }
    void SetDrawFlag(bool flag) { m_draw = flag; }

    void IncrementStackPointer() { m_stackPointer++; }
    void DecrementStackPointer() { m_stackPointer--; }

    uint16_t GetStackPointer() { return m_stackPointer; }
    uint16_t GetTopOfStack() { return m_stack[m_stackPointer]; }
    void SetTopOfStack(uint16_t val) { m_stack[m_stackPointer] = val; }

    void ClearDisplay() { m_screen = {}; }

    uint8_t GetRandomNumber();

    // Polls for SDL events. If block is true, the program will halt until an event occurs
    uint8_t PollEvents(bool block);

    // Returns true if the given key index maps to a key that is pressed
    bool IsKeyPressed(uint8_t keyIndex);

    void SetDelayTimer(uint8_t val) { m_delayTimer = val; }
    uint8_t GetDelayTimer() { return m_delayTimer; }

    void SetBeepTimer(uint8_t val) { m_beepTimer = val; }

private:
    // renders screen based on internal pixel array
    void RenderScreen(SDL_Surface* surface, SDL_Window* window) const;

    // true while emulation is active
    bool m_active;

    // opcode that we're currently executing
    uint16_t  m_currentOpcode;

    // chip8 has 4K of memmory
    std::array<uint8_t, 4096> m_memory;

    // 15 registers (0-14). V[15] is the carry flag
    std::array<uint8_t, 16> m_V;

    // index register
    uint16_t m_I;

    // program counter
    uint16_t m_PC;

    // 64px x 32px pixel screen - 2048 pixels total
    std::array<uint8_t, 64 * 32> m_screen;

    // 60hz timer
    uint8_t m_delayTimer;
    
    // 60hz timer - beeps when timer hits 0
    uint8_t  m_beepTimer;

    // used for jumps
    std::array<uint16_t, 64> m_stack;
    uint16_t m_stackPointer;

    // flag is set when the screen needs to be updated
    bool m_draw;

    // true if key is pressed, false otherwise
    std::array<volatile bool, 16> m_keyboard;

    // maps opcodes to functions that handle them
    std::unordered_map<uint16_t, std::function<void(uint16_t opc, Chip8* chip8)>> m_instructionTable;

    // represents 1 chip8 pixel
    SDL_Surface* m_pixel;

    // SDL events for keypresses
    SDL_Event m_event;

    // tick rate of the main chip8 cpu in hz
    uint16_t m_tickrate;

    // objects for random number generation
    std::mt19937 m_mersenneTwister;
    std::uniform_int_distribution<int> m_randomDist;

    const std::array<unsigned char, 80> fontset =
    {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    const std::unordered_map<SDL_Keycode, uint8_t> m_keymap;
};
