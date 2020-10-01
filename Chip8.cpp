#include <chrono>
#include <fstream>
#include "Chip8.h"
#include "Debug.h"
#include "Font.h"
#include "Instructions.h"

Chip8::Chip8() :
    m_active(true),
    m_tickrate(500),

    // first instruction is at 0x200
    m_PC(FIRST_MEMORY_LOCATION),

    // clear index register
    m_I(0),
    // clear all other registers
    m_V({}),

    // empty first opcode
    m_currentOpcode(0),

    // reset stack pointer and 0 out stack
    m_stack({}),
    m_stackPointer(0),

    // reset memmory
    m_memory({}),

    // reset timers
    m_beepTimer(0),
    m_delayTimer(0),

    // empty screen and disable draw flag
    m_draw(false),
    m_screen({}),

    // clear keyboard states and init SDL keyboard mappings
    m_keyboard({}),
    m_keymap({
        {SDLK_1, 0}, {SDLK_2, 1}, {SDLK_3, 2}, {SDLK_4, 3},
        {SDLK_q, 4}, {SDLK_w, 5}, {SDLK_e, 6}, {SDLK_r, 7},
        {SDLK_a, 8}, {SDLK_s, 9}, {SDLK_d, 10}, {SDLK_f, 11},
        {SDLK_z, 12}, {SDLK_x, 13}, {SDLK_c, 14}, {SDLK_v, 15},
        })
{
    // seed RNG for Random instruction
    std::random_device rd;
    m_mersenneTwister = std::mt19937(rd());
    m_randomDist = std::uniform_int_distribution<int>(0, 255);
}

Chip8::~Chip8()
{
    SDL_FreeSurface(m_pixel);
    SDL_Quit();
}

int Chip8::Init(int tickrate)
{
    m_tickrate = tickrate;
    
    // init fonts
    int fontIndex = 0;
    for (int i = FONT_START_ADDR; i < FONT_END_ADDR; ++i)
    {
        m_memory[i] = fontset[fontIndex++];
    }

    // clear opcode map
    m_instructionTable.clear();
    for (int i = 0; i <= 0xFFFF; ++i)
        m_instructionTable[i] = Instructions::Null;

    m_instructionTable[0x00E0] = Instructions::Clear;
    m_instructionTable[0x00EE] = Instructions::Return;
    
    // precompute valid opcodes and map them to opcode handlers
    for (int i = 0; i <= 0x0FFF; ++i)
    {
        m_instructionTable[0x1000 + i] = Instructions::Jump;
        m_instructionTable[0x2000 + i] = Instructions::Call;
        m_instructionTable[0x3000 + i] = Instructions::SkipIfEqualConst;
        m_instructionTable[0x4000 + i] = Instructions::SkipIfNotEqualConst;
        m_instructionTable[0x5000 + i] = Instructions::SkipIfEqualVal;
        m_instructionTable[0x6000 + i] = Instructions::LoadConst;
        m_instructionTable[0x7000 + i] = Instructions::AddConst;
        m_instructionTable[0x9000 + i] = Instructions::SkipIfNotEqualVal;
        m_instructionTable[0xA000 + i] = Instructions::SetIndex;
        m_instructionTable[0xB000 + i] = Instructions::JumpOffset;
        m_instructionTable[0xC000 + i] = Instructions::Random;
        m_instructionTable[0xD000 + i] = Instructions::DrawSprite;

        switch (i & 0x00F)
        {
            case 0x000:
                m_instructionTable[0x8000 + i] = Instructions::LoadVal;
                break;
            case 0x001:
                m_instructionTable[0x8000 + i] = Instructions::LoadOr;
                break;
            case 0x002:
                m_instructionTable[0x8000 + i] = Instructions::LoadAnd;
                break;
            case 0x003:
                m_instructionTable[0x8000 + i] = Instructions::LoadXor;
                break;
            case 0x004:
                m_instructionTable[0x8000 + i] = Instructions::AddVal;
                break;
            case 0x005:
                m_instructionTable[0x8000 + i] = Instructions::SubVal;
                break;
            case 0x006:
                m_instructionTable[0x8000 + i] = Instructions::ShiftRight;
                break;
            case 0x007:
                m_instructionTable[0x8000 + i] = Instructions::SubValInverse;
                break;
            case 0x00E:
                m_instructionTable[0x8000 + i] = Instructions::ShiftLeft;
                break;
        }

        switch (i & 0x0FF)
        {
            case 0x007:
                m_instructionTable[0xF000 + i] = Instructions::GetDelayTimerValue;
                break;
            case 0x00A:
                m_instructionTable[0xF000 + i] = Instructions::WaitForNextKeyPress;
                break;
            case 0x015:
                m_instructionTable[0xF000 + i] = Instructions::SetDelayTimer;
                break;
            case 0x018:
                m_instructionTable[0xF000 + i] = Instructions::SetBeepTimer;
                break;
            case 0x01E:
                m_instructionTable[0xF000 + i] = Instructions::IncrementIndex;
                break;
            case 0x029:
                m_instructionTable[0xF000 + i] = Instructions::SetIndexToFontIndex;
                break;
            case 0x033:
                m_instructionTable[0xF000 + i] = Instructions::StoreBCDValInIndex;
                break;
            case 0x055:
                m_instructionTable[0xF000 + i] = Instructions::DumpRegistersToMemory;
                break;
            case 0x065:
                m_instructionTable[0xF000 + i] = Instructions::LoadRegistersFromMemory;
                break;
            case 0x09E:
                m_instructionTable[0xE000 + i] = Instructions::SkipIfKeyPressed;
                break;
            case 0x0A1:
                m_instructionTable[0xE000 + i] = Instructions::SkipIfKeyNotPressed;
                break;
        }
    }

    // no errors
    return 0;
}

int Chip8::LoadGame(const std::string& fileName)
{
    // open file in binary mode
    // also use 'ate' flag so that we can use the tellg function to determine file size
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    // file not found
    if (!file)
        return 1;

    // get file size and then return back to the start of the file
    int fileSize = (int)file.tellg();
    file.seekg(0, file.beg);
    
    // read file bytes into buffer
    char* buffer = new char[fileSize];
    file.read(buffer, fileSize);

    // place file contents (from buffer) into chip8 memory location
    for (int i = 0; i < fileSize; ++i)
    {
        m_memory[FIRST_MEMORY_LOCATION + i] = buffer[i];
    }
    printf("Loaded %d bytes into memory\n", fileSize);

    // delete the buffer
    delete[] buffer;
    return 0;
}

void Chip8::Tick()
{    
    // fetch opcode
    m_currentOpcode = m_memory[m_PC] << 8 | m_memory[m_PC + 1];
    //if (m_currentOpcode != 0x0)
    //    printf("0x%04X\n", m_currentOpcode);

    // increment program counter by 2 to skip to the next opcode
    m_PC += 2;

    // execute opcode
    m_instructionTable[m_currentOpcode](m_currentOpcode, this);

    // tick timers (60hz always)
}

void Chip8::RenderScreen(SDL_Surface* screen, SDL_Window* window) const
{
    SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format, 0, 0, 0, 255));

    for (size_t x = 0; x < 64; ++x)
    {
        for (size_t y = 0; y < 32; ++y)
        {
            if (m_screen[y * 64 + x] == 1)
            {
                SDL_Rect pixelPos;
                pixelPos.x = x * 10;
                pixelPos.y = y * 10;

                SDL_BlitSurface(m_pixel, NULL, screen, &pixelPos);
            }
        }
    }

    SDL_UpdateWindowSurface(window);
}

uint8_t Chip8::PollEvents(bool block)
{
    if (block)
    {
        SDL_WaitEvent(&m_event);
    }
    else
    {
        SDL_PollEvent(&m_event);
    }

    switch (m_event.type)
    {
    case SDL_QUIT:
        m_active = false;
        return 0;
    case SDL_KEYDOWN:
        if (m_keymap.count(m_event.key.keysym.sym) > 0)
        {
            const uint8_t keyIndex = m_keymap.at(m_event.key.keysym.sym);
            m_keyboard[keyIndex] = true;
            return keyIndex;
        }
    case SDL_KEYUP:
        if (m_keymap.count(m_event.key.keysym.sym) > 0)
        {
            const uint8_t keyIndex = m_keymap.at(m_event.key.keysym.sym);
            if (m_keyboard[keyIndex])
            {
                m_keyboard[keyIndex] = false;
                return 0;
            }
        }
    }

    return 0;
}

void Chip8::Run()
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* screen;
    
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to start SDL: %s\n", SDL_GetError());
        return;
    }

    if (SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer))
    {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    screen = SDL_GetWindowSurface(window);

    // create a white 10x10 square to represent 1 chip8 pixel
    m_pixel = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0);
    SDL_FillRect(m_pixel, NULL, SDL_MapRGB(m_pixel->format, 255, 255, 255));

    // determine minimum time that must be spent on each tick to match the user-specified rate (or 500hz by default)
    auto minTimePerTickms = std::chrono::milliseconds(1000 / m_tickrate);

    // delay and beep timer are always 60hz
    auto minTimePerTimerms = std::chrono::milliseconds(1000 / 60);

    auto tickStartTime = std::chrono::steady_clock::now();
    auto timerStartTime = std::chrono::steady_clock::now();
    while (m_active)
    {
        auto tickTimeDiff = std::chrono::steady_clock::now() - tickStartTime;

        if (tickTimeDiff >= minTimePerTickms)
        {
            tickStartTime = std::chrono::steady_clock::now();
            //printf("TIME DIFF: %lld\n", tickTimeDiff.count());

            // check for keyboard presses without blocking
            PollEvents(false);

            // execute next instruction
            Tick();

            if (m_PC >= 4096)
            {
                printf("Memory out of bounds!");
                break;
            }

            // screen needs to be updated
            if (m_draw)
            {
                RenderScreen(screen, window);
            }
        }

        auto timerTimeDiff = std::chrono::steady_clock::now() - timerStartTime;
        if (timerTimeDiff >= minTimePerTimerms)
        {
            if (m_delayTimer > 0)
                m_delayTimer--;

            if (m_beepTimer >= 1)
            {
                m_beepTimer--;
                if (m_beepTimer == 0)
                    printf("BEEP\n");
            }
        }
    }
}

void Chip8::SetProgramCounter(uint16_t pc)
{
    m_PC = pc;
}

uint8_t Chip8::GetRegister(uint8_t regIndex) const
{
    if (regIndex > m_V.size())
    {
        printf("GetRegister: Invalid register specified (%d)\n", regIndex);
        return 0;
    }

    return m_V[regIndex];
}

void Chip8::SetRegister(uint8_t regIndex, uint8_t val)
{
    if (regIndex > m_V.size())
    {
        printf("SetRegister: Invalid register specified (%d)\n", regIndex);
        return;
    }

    m_V[regIndex] = val;
}

uint8_t Chip8::GetMemory(uint16_t memIndex) const
{
    if (memIndex > m_memory.size())
    {
        printf("GetMemory: Invalid register specified (%d)\n", memIndex);
        return 0;
    }

    Debug::Log("\tGetMemory: memory[0x%X] = 0x%X\n", memIndex, m_memory[memIndex]);
    return m_memory[memIndex];
}

void Chip8::SetMemory(uint16_t memIndex, uint8_t val)
{
    if (memIndex > m_memory.size())
    {
        printf("SetMemory: Invalid register specified (%d)\n", memIndex);
        return;
    }

    m_memory[memIndex] = val;
    Debug::Log("\tSetMemory: memory[0x%X] = 0x%X\n", memIndex, m_memory[memIndex]);
    return;
}

uint16_t Chip8::GetIndex() const
{
    return m_I;
}

void Chip8::SetIndex(uint16_t val)
{
    m_I = val;
}

bool Chip8::GetPixelStatus(uint16_t pixel) const
{
    if (pixel > m_screen.size())
    {
        printf("GetPixelStatus: Invalid pixel specified (%d)\n", pixel);
        return false;
    }

    return m_screen[pixel] == 1;
}

void Chip8::XorPixel(uint16_t pixel)
{
    if (pixel > m_screen.size())
    {
        printf("SetPixelStatus: Invalid pixel specified (%d)\n", pixel);
        return;
    }

    m_screen[pixel] ^= 1;
}

uint8_t Chip8::GetRandomNumber()
{
    return m_randomDist(m_mersenneTwister);
}

bool Chip8::IsKeyPressed(uint8_t keyIndex)
{
    if (keyIndex >= m_keyboard.size())
    {
        printf("IsKeyPressed: keyIndex is greater than %d: %d\n", m_keyboard.size(), keyIndex);
        return false;
    }

    return m_keyboard[keyIndex];
}
