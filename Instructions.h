#pragma once
#include "Chip8.h"

class Instructions
{
public:
    // Do nothing
    static void Null(uint16_t opc, Chip8* chip8);

    // 00E0 - Clear the display
    static void Clear(uint16_t opc, Chip8* chip8);

    // 00EE - Return from a subroutine.
    static void Return(uint16_t opc, Chip8* chip8);

    // 1nnn - Jump to location nnn.
    static void Jump(uint16_t opc, Chip8* chip8);

    // 2nnn - Call subroutine at nnn.
    static void Call(uint16_t opc, Chip8* chip8);

    // 3xkk - Skip next instruction if Vx = kk.
    static void SkipIfEqualConst(uint16_t opc, Chip8* chip8);

    // 4xkk - Skip next instruction if Vx != kk.
    static void SkipIfNotEqualConst(uint16_t opc, Chip8* chip8);

    // 5xy0 - Skip next instruction if Vx = Vy.
    static void SkipIfEqualVal(uint16_t opc, Chip8* chip8);

    // 6xkk - Set Vx = kk.
    static void LoadConst(uint16_t opc, Chip8* chip8);

    // 7xkk - Set Vx = Vx + kk.
    static void AddConst(uint16_t opc, Chip8* chip8);

    // 8xy0 - Set Vx = Vy.
    static void LoadVal(uint16_t opc, Chip8* chip8);

    // 8XY1 - Sets VX to VX or VY. (Bitwise OR operation) 
    static void LoadOr(uint16_t opc, Chip8* chip8);

    // 8XY2 - Sets VX to VX and VY. (Bitwise AND operation) 
    static void LoadAnd(uint16_t opc, Chip8* chip8);

    // 8XY3 - Sets VX to VX xor VY. 
    static void LoadXor(uint16_t opc, Chip8* chip8);

    // 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. 
    static void AddVal(uint16_t opc, Chip8* chip8);

    // 8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
    static void SubVal(uint16_t opc, Chip8* chip8);

    // 8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[2] 
    static void ShiftRight(uint16_t opc, Chip8* chip8);

    // 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
    static void SubValInverse(uint16_t opc, Chip8* chip8);

    // 8XYE - Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
    static void ShiftLeft(uint16_t opc, Chip8* chip8);

    // 9XY0  - Skips the next instruction if VX doesn't equal VY.
    static void SkipIfNotEqualVal(uint16_t opc, Chip8* chip8);

    // ANNN - Sets I to the address NNN. 
    static void SetIndex(uint16_t opc, Chip8* chip8);

    // BNNN - Jumps to the address NNN plus V0. 
    static void JumpOffset(uint16_t opc, Chip8* chip8);

    // CXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. 
    static void Random(uint16_t opc, Chip8* chip8);

    // DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
    // Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction.
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
    static void DrawSprite(uint16_t opc, Chip8* chip8);

    // EX9E - Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block) 
    static void SkipIfKeyPressed(uint16_t opc, Chip8* chip8);

    // EXA1 - Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block) 
    static void SkipIfKeyNotPressed(uint16_t opc, Chip8* chip8);

    // FX07 - Sets VX to the value of the delay timer. 
    static void GetDelayTimerValue(uint16_t opc, Chip8* chip8);

    // FX0A - A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
    static void WaitForNextKeyPress(uint16_t opc, Chip8* chip8);

    // FX15 - Sets the delay timer to VX. 
    static void SetDelayTimer(uint16_t opc, Chip8* chip8);

    // FX18 - Sets the sound timer to VX. 
    static void SetBeepTimer(uint16_t opc, Chip8* chip8);

    // FX1E - Adds VX to I.
    static void IncrementIndex(uint16_t opc, Chip8* chip8);

    // FX29 - Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font. 
    static void SetIndexToFontIndex(uint16_t opc, Chip8* chip8);

    // FX33 - Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I,
    // the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX,
    // place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) 
    static void StoreBCDValInIndex(uint16_t opc, Chip8* chip8);

    // FX55 - Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. 
    static void DumpRegistersToMemory(uint16_t opc, Chip8* chip8);

    // FX65 - Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified. 
    static void LoadRegistersFromMemory(uint16_t opc, Chip8* chip8);
};
