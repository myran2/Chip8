#include <iostream>
#include "Debug.h"
#include "Instructions.h"

void Instructions::Null(uint16_t opc, Chip8* chip8)
{
    //printf("STUB: null\n");
}

void Instructions::Clear(uint16_t opc, Chip8* chip8)
{
    Debug::Log("0x%04X: Clear\n", opc);
    
    chip8->ClearDisplay();
    //TODO: set draw flag here too?
}

void Instructions::Return(uint16_t opc, Chip8* chip8)
{
    Debug::Log("0x%04X: Return\n", opc);

    chip8->SetProgramCounter(chip8->GetTopOfStack());
    chip8->DecrementStackPointer();
}

void Instructions::Jump(uint16_t opc, Chip8* chip8)
{
    uint16_t addr = opc & 0x0FFF;
    Debug::Log("0x%04X: Jump 0x%03x\n", opc, addr);
    chip8->SetProgramCounter(addr);
    return;
}

void Instructions::Call(uint16_t opc, Chip8* chip8)
{
    uint16_t addr = opc & 0x0FFF;
    Debug::Log("0x%04X: Call 0x%03x\n", opc, addr);

    chip8->IncrementStackPointer();
    chip8->SetTopOfStack(chip8->GetProgramCounter());
    chip8->SetProgramCounter(addr);
}

void Instructions::SkipIfEqualConst(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);
    uint8_t val = opc & 0x00FF;

    Debug::Log("0x%04X: SkipIfEqualConst V[%d] = 0x%02x =?= 0x%02x: ", opc, regIndex, regVal, val);

    if (val == regVal)
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("Yes\n");
    }
    else
        Debug::Log("No\n");

    return;
}

void Instructions::SkipIfNotEqualConst(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);
    uint8_t val = opc & 0x00FF;

    Debug::Log("0x%04X: SkipIfNotEqualConst V[%d] = 0x%02x =?= 0x%02x: ", opc, regIndex, regVal, val);

    if (val != regVal)
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("No\n");
    }
    else
        Debug::Log("Yes\n");

    return;
}

void Instructions::SkipIfEqualVal(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: SkipIfEqualVal V[%d] = 0x%02x =?= V[%d] = 0x%02x: ", opc, regIndex1, regVal1, regIndex2, regVal2);

    if (regVal1 == regVal2)
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("Yes\n");
    }
    else
        Debug::Log("No\n");

    return;
}

void Instructions::LoadConst(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t val = opc & 0x00FF;

    Debug::Log("0x%04X: LoadConst V[%d] = 0x%02x\n", opc, regIndex, val);

    chip8->SetRegister(regIndex, val);
}

void Instructions::AddConst(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t addVal = opc & 0x00FF;
    uint8_t currentVal = chip8->GetRegister(regIndex);

    Debug::Log("0x%04X: AddConst V[%d] = 0x%02x + 0x%02x\n", opc, regIndex, currentVal, addVal);

    chip8->SetRegister(regIndex, currentVal + addVal);
}

void Instructions::LoadVal(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: LoadVal V[%d] = V[%d] = 0x%02x\n", opc, regIndex1, regIndex2, regVal2);

    chip8->SetRegister(regIndex1, regVal2);
}

void Instructions::LoadOr(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: LoadOr V[%d] = 0x%02x | (V[%d] = 0x%02x)\n", opc, regIndex1, regVal1, regIndex2, regVal2);

    chip8->SetRegister(regIndex1, regVal1 | regVal2);
}

void Instructions::LoadAnd(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: LoadAnd V[%d] = 0x%02x & (V[%d] = 0x%02x)\n", opc, regIndex1, regVal1, regIndex2, regVal2);

    chip8->SetRegister(regIndex1, regVal1 & regVal2);
}

void Instructions::LoadXor(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: LoadXor V[%d] = 0x%02x ^ (V[%d] = 0x%02x)\n", opc, regIndex1, regVal1, regIndex2, regVal2);

    chip8->SetRegister(regIndex1, regVal1 ^ regVal2);
}

void Instructions::AddVal(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: AddVal (V[%d] = 0x%02x) + (V[%d] = 0x%02x)\n", opc, regIndex1, regVal1, regIndex2, regVal2);

    uint16_t sum = regVal1 + regVal2;

    // Set carry flag to 1 if sum requires more than 8 bits (> 255)
    if (sum > 0xFF)
        chip8->SetRegister(0xF, 1);

    chip8->SetRegister(regIndex1, (sum & 0xFF));
}

void Instructions::SubVal(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: SubVal (V[%d] = 0x%02x) - (V[%d] = 0x%02x)\n", opc, regIndex1, regVal1, regIndex2, regVal2);

    // Set carry flag to 1 if the result of subtraction will be negative
    if (regVal2 > regVal1)
        chip8->SetRegister(0xF, 0);
    else
        chip8->SetRegister(0xF, 1);

    uint8_t diff = regVal1 - regVal2;
    chip8->SetRegister(regIndex1, diff);
}

void Instructions::ShiftRight(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    // set carry flag if least significant bit of the value is 1
    if ((regVal & 0x1) == 1)
        chip8->SetRegister(0xF, 1);
    else
        chip8->SetRegister(0xF, 0);

    uint8_t res = (regVal >> 1);
    Debug::Log("0x%04X: ShiftRight (V[%d] = 0x%02x) >> 1 = 0x%02x\n", opc, regIndex, regVal, res);
    chip8->SetRegister(regIndex, res);
}

void Instructions::SubValInverse(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint8_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint8_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: SubValInverse (V[%d] = 0x%02x) - (V[%d] = 0x%02x)\n", opc, regIndex2, regVal2, regIndex1, regVal1);

    // Set carry flag to 1 if the result of subtraction will be negative
    if (regVal1 > regVal2)
        chip8->SetRegister(0xF, 0);
    else
        chip8->SetRegister(0xF, 1);

    uint8_t diff = regVal2 - regVal1;
    chip8->SetRegister(regIndex1, diff);
}

void Instructions::ShiftLeft(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    // set V[F] to the most significant bit
    chip8->SetRegister(0xF, (regVal & 0x80) >> 7);

    uint8_t res = (regVal << 1);
    Debug::Log("0x%04X: ShiftLeft V[%d] = ((V[%d]) 0x%04x << 1) = 0x%04x\n", opc, regIndex, regIndex, regVal, res);
    chip8->SetRegister(regIndex, res);
}

void Instructions::SkipIfNotEqualVal(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex1 = (opc & 0x0F00) >> 8;
    uint16_t regVal1 = chip8->GetRegister(regIndex1);

    uint8_t regIndex2 = (opc & 0x00F0) >> 4;
    uint16_t regVal2 = chip8->GetRegister(regIndex2);

    Debug::Log("0x%04X: SkipIfNotEqualVal V[%d] = 0x%02x =?= V[%d] = 0x%02x: ", opc, regIndex1, regVal1, regIndex2, regVal2);

    if (regVal1 != regVal2)
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("No\n");
    }
    else
        Debug::Log("Yes\n");

    return;
}

void Instructions::SetIndex(uint16_t opc, Chip8* chip8)
{
    uint16_t addr = opc & 0x0FFF;

    Debug::Log("0x%04X: SetIndex 0x%X\n", opc, addr);

    chip8->SetIndex(addr);
}

void Instructions::JumpOffset(uint16_t opc, Chip8* chip8)
{
    uint16_t baseAddr = opc & 0x0FFF;
    uint8_t v0 = chip8->GetRegister(0x0);
    uint16_t addr = baseAddr + v0;
    Debug::Log("0x%04X: JumpOffset 0x%03x (0x%03x + V[0](0x%02x))\n", opc, addr, baseAddr, v0);
    chip8->SetProgramCounter(baseAddr + v0);
    return;
}

void Instructions::Random(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t nn = (opc & 0x00FF);

    uint8_t rand = 0;
    uint8_t val = rand & nn;

    Debug::Log("0x%04X: Random V[%d] = 0x%02X & 0x%02X = 0x%04X\n", opc, regIndex, rand, nn, val);
    chip8->SetRegister(regIndex, val);
}

void Instructions::DrawSprite(uint16_t opc, Chip8* chip8)
{
    uint8_t xRegIndex = (opc & 0x0F00) >> 8;
    uint8_t yRegIndex = (opc & 0x00F0) >> 4;
    Debug::Log("0x%04X: DrawSprite: xReg(%d) yReg(%d)\n", opc, xRegIndex, yRegIndex);
    
    uint16_t x = chip8->GetRegister(xRegIndex);
    uint16_t y = chip8->GetRegister(yRegIndex);
    uint16_t height = opc & 0x000F;
    Debug::Log("0x%04X: DrawSprite: x(%d) y(%d) height(%d)\n", opc, x, y, height);
    uint16_t pixel;

    // reset collision flag to 0
    chip8->SetRegister(0xF, 0);

    for (int yInd = 0; yInd < height; yInd++)
    {
        pixel = chip8->GetMemory(chip8->GetIndex() + yInd);
        for (int xInd = 0; xInd < 8; xInd++)
        {
            if ((pixel & (0x80 >> xInd)) != 0)
            {
                // check for a collision (pixel is already 1)
                if ((chip8->GetPixelStatus(x + xInd + ((y + yInd) * 64))) == 1)
                    chip8->SetRegister(0xF, 1);

                chip8->XorPixel(x + xInd + ((y + yInd) * 64));
            }
        }
    }

    chip8->SetDrawFlag(true);
    return;
}

void Instructions::SkipIfKeyPressed(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    Debug::Log("0x%04X: SkipIfKeyPressed V[%d] = %d : ", opc, regIndex, regVal);

    if (chip8->IsKeyPressed(regVal))
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("Pressed\n");
    }
    else
    {
        Debug::Log("Not Pressed\n");
    }
}

void Instructions::SkipIfKeyNotPressed(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    Debug::Log("0x%04X: SkipIfNotKeyPressed V[%d] = %d : ", opc, regIndex, regVal);

    if (!chip8->IsKeyPressed(regVal))
    {
        chip8->SetProgramCounter(chip8->GetProgramCounter() + 2);
        Debug::Log("Not Pressed\n");
    }
    else
    {
        Debug::Log("Pressed\n");
    }
}

void Instructions::GetDelayTimerValue(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;

    Debug::Log("0x%04X: GetDelayTimer: V[%d] = %d\n", opc, regIndex, chip8->GetDelayTimer());
    chip8->SetRegister(regIndex, chip8->GetDelayTimer());
}

void Instructions::WaitForNextKeyPress(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;

    Debug::Log("0x%04X: WaitForNextKeyPress V[%d] = ... ", opc, regIndex);
    uint8_t key = chip8->PollEvents(true);

    chip8->SetRegister(regIndex, key);
    Debug::Log("%d\n", key);
}

void Instructions::SetDelayTimer(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    Debug::Log("0x%04X: SetDelayTimer = V[%d] = %d\n", opc, regIndex, regVal);
    chip8->SetDelayTimer(regVal);
}

void Instructions::SetBeepTimer(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    Debug::Log("0x%04X: SetBeepTimer = V[%d] = %d\n", opc, regIndex, regVal);
    chip8->SetBeepTimer(regVal);
}

void Instructions::IncrementIndex(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);
    
    Debug::Log("0x%04X: Index += V[%d] (%d)\n", opc, regIndex, regVal);
    chip8->SetIndex(chip8->GetIndex() + regVal);
}

void Instructions::SetIndexToFontIndex(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);
    uint16_t addr = (regVal * 5) + FONT_START_ADDR;
    
    Debug::Log("0x%04X: SetIndexToFontIndex index for V[%d] (0x%X) is 0x%04X\n", opc, regIndex, regVal, addr);
    chip8->SetIndex(addr);
}

void Instructions::StoreBCDValInIndex(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint8_t regVal = chip8->GetRegister(regIndex);

    uint8_t a = regVal / 100;
    uint8_t b = (regVal / 10) % 10;
    uint8_t c = regVal % 10;

    Debug::Log("0x%04X: StoreBCDValInIndex V[%d] = %d => %d %d %d\n", opc, regIndex, regVal, a, b, c);
    chip8->SetMemory(chip8->GetIndex(), a);
    chip8->SetMemory(chip8->GetIndex() + 1, b);
    chip8->SetMemory(chip8->GetIndex() + 2, c);
}

void Instructions::DumpRegistersToMemory(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint16_t I = chip8->GetIndex();

    Debug::Log("0x%04X: DumpRegistersToMemory V[0] to V[%d] starting at Memory[0x%X]\n", opc, regIndex, I);
    for (int i = 0; i <= regIndex; ++i)
    {
        chip8->SetMemory(I + i, chip8->GetRegister(i));
        Debug::Log("\t[0x%X] = V[%d] = 0x%X\n", I+i, i, chip8->GetRegister(i));
    }
}

void Instructions::LoadRegistersFromMemory(uint16_t opc, Chip8* chip8)
{
    uint8_t regIndex = (opc & 0x0F00) >> 8;
    uint16_t I = chip8->GetIndex();

    Debug::Log("0x%04X: LoadRegistersFromMemory V[0] to V[%d] starting at Memory[0x%X]\n", opc, regIndex, I);
    for (int i = 0; i <= regIndex; ++i)
    {
        chip8->SetRegister(i, chip8->GetMemory(I + i));
        Debug::Log("\tV[%d] = 0x%X\n", i, chip8->GetMemory(I+i));
    }
}
