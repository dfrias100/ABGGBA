#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include "AccessType.h"

class Memory {
public:
    void WriteWord(uint32_t unAddress, uint32_t unData, AccessType armAccessType);
    void WriteHalfWord(uint32_t unAddress, uint16_t unData, AccessType armAccessType);
    void WriteByte(uint32_t unAddress, uint8_t unData, AccessType armAccessType);

    uint32_t ReadWord(uint32_t unAddress, AccessType armAccessType);
    uint16_t ReadHalfWord(uint32_t unAddress, AccessType armAccessType);
    uint8_t  ReadByte(uint32_t unAddress, AccessType armAccessType);
private:
    uint32_t m_aBiosRom[16 * 0x400];
    uint32_t m_aOnBoardWorkRam[256 * 0x400];
    uint32_t m_aOnChipWorkRam[32 * 0x400];
};

#endif