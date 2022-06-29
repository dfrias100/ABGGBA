#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>

#include "../PPU/PPU.h"
#include "AccessType.h"

class PPU;

class Memory {
public:
    void InitTest();
    void WriteWord(uint32_t unAddress, uint32_t unData, AccessType armAccessType);
    void WriteHalfWord(uint32_t unAddress, uint16_t usnData, AccessType armAccessType);
    void WriteByte(uint32_t unAddress, uint8_t ubyData, AccessType armAccessType);

    uint32_t ReadWord(uint32_t unAddress, AccessType armAccessType);
    uint16_t ReadHalfWord(uint32_t unAddress, AccessType armAccessType);
    uint8_t  ReadByte(uint32_t unAddress, AccessType armAccessType);

    void ConnectPpu(PPU* ppu);
private:
    PPU* m_Ppu;
    uint8_t m_aBiosRom[16 * 0x400];
    uint8_t m_aOnBoardWorkRam[256 * 0x400];
    uint8_t m_aOnChipWorkRam[32 * 0x400];

    // TEMPORARY
    uint8_t m_aGamePakRom[32 * (0x400 * 0x400)];
};

#endif