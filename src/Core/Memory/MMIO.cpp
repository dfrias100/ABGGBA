#include "Memory.h"

// It was this or write all the offsets myself...I say let the compiler do the work for me
#define SWITCH_CASE_BASE(Case, Offset, ...)    \
    case Case + Offset: {                      \
        constexpr auto AddressOffset = Offset; \
        __VA_ARGS__;                           \
        break;                                 \
    }                                          \

#define SWITCH_CASE_HALF_OFFSET(Case, Offset, ...) SWITCH_CASE_BASE(Case, Offset, __VA_ARGS__)        SWITCH_CASE_BASE(Case, Offset + 1, __VA_ARGS__)
#define SWITCH_CASE_WORD_OFFSET(Case, Offset, ...) SWITCH_CASE_HALF_OFFSET(Case, Offset, __VA_ARGS__) SWITCH_CASE_HALF_OFFSET(Case, Offset + 2, __VA_ARGS)

#define SWITCH_CASE_HALF(Case, ...) SWITCH_CASE_HALF_OFFSET(Case, 0, __VA_ARGS__)
#define SWITCH_CASE_WORD(Case, ...) SWITCH_CASE_WORD_OFFSET(Case, 0, __VA_ARGS__)

void Memory::WriteByteToIo(uint32_t unAddress, uint8_t ubyData) {
    switch (unAddress) {
        SWITCH_CASE_HALF(PPU::IoRegister::IO_DISPCNT,  m_Ppu->WriteByteToRegister(PPU::IoRegister::IO_DISPCNT, ubyData, AddressOffset))
        SWITCH_CASE_HALF(PPU::IoRegister::IO_DISPSTAT, m_Ppu->WriteByteToRegister(PPU::IoRegister::IO_DISPSTAT, ubyData, AddressOffset))
    }
}

uint8_t Memory::ReadByteFromIo(uint32_t unAddress) {
    uint8_t ubyData = 0x00;
    switch (unAddress) {
        SWITCH_CASE_HALF(PPU::IoRegister::IO_DISPCNT, ubyData = m_Ppu->ReadByteFromRegister(PPU::IoRegister::IO_DISPCNT, AddressOffset))
        SWITCH_CASE_HALF(PPU::IoRegister::IO_DISPSTAT, ubyData = m_Ppu->ReadByteFromRegister(PPU::IoRegister::IO_DISPSTAT, AddressOffset))
    }
    return ubyData;
}