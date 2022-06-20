#include "Memory.h"

#include <iostream>
#include <fstream>
#include <string>

void Memory::InitTest() {
    std::fill(std::begin(m_aOnBoardWorkRam), std::end(m_aOnBoardWorkRam), 0);
    std::fill(std::begin(m_aOnChipWorkRam), std::end(m_aOnChipWorkRam), 0);
    std::fill(std::begin(m_aBiosRom), std::end(m_aBiosRom), 0);
    std::fill(std::begin(m_aGamePakRom), std::end(m_aGamePakRom), 0);

    const std::string szTestRomFile = "FuzzARM.gba";
    const std::string szBiosRomFile = "gba_bios.bin";

    std::ifstream ifsTestRomStream(szTestRomFile, std::ios::binary);
    std::ifstream ifsBiosRomStream(szBiosRomFile, std::ios::binary);

    if (ifsTestRomStream.is_open()) {
	std::streampos unRomSizeInBytes = ifsTestRomStream.tellg();
	ifsTestRomStream.seekg(0, std::ios::end);
	unRomSizeInBytes = ifsTestRomStream.tellg() - unRomSizeInBytes;
	ifsTestRomStream.clear();
	ifsTestRomStream.seekg(0, std::ios::beg);

	ifsTestRomStream.read(reinterpret_cast<char*>(m_aGamePakRom), unRomSizeInBytes);
	ifsBiosRomStream.read(reinterpret_cast<char*>(m_aBiosRom), 16ULL * 0x400);
    }

    ifsTestRomStream.close();
    ifsBiosRomStream.close();
}

void Memory::WriteWord(uint32_t unAddress, uint32_t unData, AccessType armAccessType) {
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unAddress & 0x3FFFF]       =  unData        & 0xFF;
	m_aOnBoardWorkRam[(unAddress + 1) & 0x3FFFF] = (unData >>  8) & 0xFF;
	m_aOnBoardWorkRam[(unAddress + 2) & 0x3FFFF] = (unData >> 16) & 0xFF;
	m_aOnBoardWorkRam[(unAddress + 3) & 0x3FFFF] = (unData >> 24) & 0xFF;
	break;
    case 0x03:
	m_aOnChipWorkRam[unAddress & 0x7FFF] = unData & 0xFF;
	m_aOnChipWorkRam[(unAddress + 1) & 0x7FFF] = (unData >> 8) & 0xFF;
	m_aOnChipWorkRam[(unAddress + 2) & 0x7FFF] = (unData >> 16) & 0xFF;
	m_aOnChipWorkRam[(unAddress + 3) & 0x7FFF] = (unData >> 24) & 0xFF;
	break;
    // TODO: IO and SRAM
    }
}

void Memory::WriteHalfWord(uint32_t unAddress, uint16_t usnData, AccessType armAccessType) {
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unAddress & 0x3FFFF] = usnData & 0xFF;
	m_aOnBoardWorkRam[(unAddress + 1) & 0x3FFFF] = (usnData >> 8) & 0xFF;
	break;
    case 0x03:
	m_aOnChipWorkRam[unAddress & 0x7FFF] = usnData & 0xFF;
	m_aOnChipWorkRam[(unAddress + 1) & 0x7FFF] = (usnData >> 8) & 0xFF;
	break;
	// TODO: IO and SRAM
    }
}

void Memory::WriteByte(uint32_t unAddress, uint8_t ubyData, AccessType armAccessType) {
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unAddress & 0x3FFFF] = ubyData;
	break;
    case 0x03:
	m_aOnChipWorkRam[unAddress & 0x7FFF] = ubyData;
	break;
	// TODO: IO and SRAM
    }
}

uint32_t Memory::ReadWord(uint32_t unAddress, AccessType armAccessType) {
    uint32_t unData = 0x00;
    switch (unAddress >> 24) {
    case 0x00:
	unData |= m_aBiosRom[unAddress & 0x3FFF];
	unData |= m_aBiosRom[(unAddress + 1) & 0x3FFF] << 8;
	unData |= m_aBiosRom[(unAddress + 2) & 0x3FFF] << 16;
	unData |= m_aBiosRom[(unAddress + 3) & 0x3FFF] << 24;
	break;
    case 0x02:
	// Technically cannot read if greater than 0x3FFFF, fix this later
	unData |= m_aOnBoardWorkRam[unAddress & 0x3FFFF];
	unData |= m_aOnBoardWorkRam[(unAddress + 1) & 0x3FFFF] << 8;
	unData |= m_aOnBoardWorkRam[(unAddress + 2) & 0x3FFFF] << 16;
	unData |= m_aOnBoardWorkRam[(unAddress + 3) & 0x3FFFF] << 24;
	break;
    case 0x03:
	unData |= m_aOnChipWorkRam[unAddress & 0x7FFF];
	unData |= m_aOnChipWorkRam[(unAddress + 1) & 0x7FFF] << 8;
	unData |= m_aOnChipWorkRam[(unAddress + 2) & 0x7FFF] << 16;
	unData |= m_aOnChipWorkRam[(unAddress + 3) & 0x7FFF] << 24;
	break;
	// TODO: IO and SRAM
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	unData |= m_aGamePakRom[unAddress & 0x01FFFFFF];
	unData |= m_aGamePakRom[(unAddress + 1) & 0x01FFFFFF] << 8;
	unData |= m_aGamePakRom[(unAddress + 2) & 0x01FFFFFF] << 16;
	unData |= m_aGamePakRom[(unAddress + 3) & 0x01FFFFFF] << 24;
	break;
    }
    return unData;
}

uint16_t Memory::ReadHalfWord(uint32_t unAddress, AccessType armAccessType) {
    uint16_t usnData = 0x00;
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS
	usnData |= m_aBiosRom[unAddress & 0x3FFF];
	usnData |= m_aBiosRom[(unAddress + 1) & 0x3FFF] << 8;
	break;
    case 0x02:
	// Technically cannot read if greater than 0x3FFFF, fix this later
	usnData |= m_aOnBoardWorkRam[unAddress & 0x3FFFF];
	usnData |= m_aOnBoardWorkRam[(unAddress + 1) & 0x3FFFF] << 8;
	break;
    case 0x03:
	usnData |= m_aOnChipWorkRam[unAddress & 0x7FFF];
	usnData |= m_aOnChipWorkRam[(unAddress + 1) & 0x7FFF] << 8;
	break;
	// TODO: IO and SRAM
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	usnData |= m_aGamePakRom[unAddress & 0x001FFFFF];
	usnData |= m_aGamePakRom[(unAddress + 1) & 0x001FFFFF] << 8;
	break;
    }
    return usnData;
}

uint8_t Memory::ReadByte(uint32_t unAddress, AccessType armAccessType) {
    uint8_t ubyData = 0x00;
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS
	ubyData |= m_aBiosRom[unAddress & 0x3FFF];
	break;
    case 0x02:
	// Technically cannot read if greater than 0x3FFF, fix this later
	ubyData = m_aOnBoardWorkRam[unAddress & 0x3FFFF];
	break;
    case 0x03:
	ubyData = m_aOnChipWorkRam[unAddress & 0x7FFF];
	break;
	// TODO: IO and SRAM
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	ubyData = m_aGamePakRom[unAddress & 0x001FFFFF];
	break;
    }
    return ubyData;
}
