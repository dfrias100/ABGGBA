#include "Memory.h"

#include <iostream>
#include <fstream>
#include <string>

void Memory::InitTest() {
    std::fill(std::begin(m_aOnBoardWorkRam), std::end(m_aOnBoardWorkRam), 0);
    std::fill(std::begin(m_aOnChipWorkRam), std::end(m_aOnChipWorkRam), 0);
    std::fill(std::begin(m_aBiosRom), std::end(m_aBiosRom), 0);
    std::fill(std::begin(m_aGamePakRom), std::end(m_aGamePakRom), 0);

    const std::string szTestRomFile = "pageflip.mb.gba";
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
    uint32_t unWordAlignedAddress = unAddress & ~0b11;

    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unWordAlignedAddress & 0x3FFFF]       =  unData        & 0xFF;
	m_aOnBoardWorkRam[(unWordAlignedAddress + 1) & 0x3FFFF] = (unData >>  8) & 0xFF;
	m_aOnBoardWorkRam[(unWordAlignedAddress + 2) & 0x3FFFF] = (unData >> 16) & 0xFF;
	m_aOnBoardWorkRam[(unWordAlignedAddress + 3) & 0x3FFFF] = (unData >> 24) & 0xFF;
	m_pScheduler->m_ulSystemClock += 6;
	break;
    case 0x03:
	m_aOnChipWorkRam[unWordAlignedAddress & 0x7FFF]       = unData & 0xFF;
	m_aOnChipWorkRam[(unWordAlignedAddress + 1) & 0x7FFF] = (unData >> 8) & 0xFF;
	m_aOnChipWorkRam[(unWordAlignedAddress + 2) & 0x7FFF] = (unData >> 16) & 0xFF;
	m_aOnChipWorkRam[(unWordAlignedAddress + 3) & 0x7FFF] = (unData >> 24) & 0xFF;
	m_pScheduler->m_ulSystemClock++;
	break;
    // TODO: IO and SRAM
    case 0x04:
	WriteByteToIo(unWordAlignedAddress, unData & 0xFF);
	WriteByteToIo(unWordAlignedAddress + 1, (unData >> 8) & 0xFF);
	WriteByteToIo(unWordAlignedAddress + 1, (unData >> 16) & 0xFF);
	WriteByteToIo(unWordAlignedAddress + 1, (unData >> 24) & 0xFF);
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x05:
    case 0x06:
    case 0x07:
	m_Ppu->WriteWord(unWordAlignedAddress, unData);
	break;
    }
}

void Memory::WriteHalfWord(uint32_t unAddress, uint16_t usnData, AccessType armAccessType) {
    uint32_t unHalfWordAlignedAddress = unAddress & ~0b1;
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unHalfWordAlignedAddress & 0x3FFFF] = usnData & 0xFF;
	m_aOnBoardWorkRam[(unHalfWordAlignedAddress + 1) & 0x3FFFF] = (usnData >> 8) & 0xFF;
	m_pScheduler->m_ulSystemClock += 3;
	break;
    case 0x03:
	m_aOnChipWorkRam[unHalfWordAlignedAddress & 0x7FFF] = usnData & 0xFF;
	m_aOnChipWorkRam[(unHalfWordAlignedAddress + 1) & 0x7FFF] = (usnData >> 8) & 0xFF;
	m_pScheduler->m_ulSystemClock++;
	break;
	// TODO: IO and SRAM
    case 0x04:
	WriteByteToIo(unHalfWordAlignedAddress, usnData & 0xFF);
	WriteByteToIo(unHalfWordAlignedAddress + 1, (usnData >> 8) & 0xFF);
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x05:
    case 0x06:
    case 0x07:
	m_Ppu->WriteHalf(unHalfWordAlignedAddress, usnData);
	break;
    }
}

void Memory::WriteByte(uint32_t unAddress, uint8_t ubyData, AccessType armAccessType) {
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS - Do not write!!
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x02:
	// Technically cannot write if greater than 0x3FFFF, fix this later
	m_aOnBoardWorkRam[unAddress & 0x3FFFF] = ubyData;
	m_pScheduler->m_ulSystemClock += 3;
	break;
    case 0x03:
	m_aOnChipWorkRam[unAddress & 0x7FFF] = ubyData;
	m_pScheduler->m_ulSystemClock++;
	break;
	// TODO: IO and SRAM
    case 0x04:
	WriteByteToIo(unAddress, ubyData);
	m_pScheduler->m_ulSystemClock++;
	break;
    }
}

uint32_t Memory::ReadWord(uint32_t unAddress, AccessType armAccessType) {
    uint32_t unWordAlignedAddress = unAddress & ~0b11;
    uint32_t unData = 0x00;
    switch (unAddress >> 24) {
    case 0x00:
	unData |= m_aBiosRom[unWordAlignedAddress & 0x3FFF];
	unData |= m_aBiosRom[(unWordAlignedAddress + 1) & 0x3FFF] << 8;
	unData |= m_aBiosRom[(unWordAlignedAddress + 2) & 0x3FFF] << 16;
	unData |= m_aBiosRom[(unWordAlignedAddress + 3) & 0x3FFF] << 24;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x02:
	// Technically cannot read if greater than 0x3FFFF, fix this later
	unData |= m_aOnBoardWorkRam[unWordAlignedAddress & 0x3FFFF];
	unData |= m_aOnBoardWorkRam[(unWordAlignedAddress + 1) & 0x3FFFF] << 8;
	unData |= m_aOnBoardWorkRam[(unWordAlignedAddress + 2) & 0x3FFFF] << 16;
	unData |= m_aOnBoardWorkRam[(unWordAlignedAddress + 3) & 0x3FFFF] << 24;
	m_pScheduler->m_ulSystemClock += 6;
	break;

    case 0x03:
	unData |= m_aOnChipWorkRam[unWordAlignedAddress & 0x7FFF];
	unData |= m_aOnChipWorkRam[(unWordAlignedAddress + 1) & 0x7FFF] << 8;
	unData |= m_aOnChipWorkRam[(unWordAlignedAddress + 2) & 0x7FFF] << 16;
	unData |= m_aOnChipWorkRam[(unWordAlignedAddress + 3) & 0x7FFF] << 24;
	m_pScheduler->m_ulSystemClock++;
	break;
	
	// TODO: IO and SRAM
    case 0x04:
	unData |= ReadByteFromIo(unWordAlignedAddress);
	unData |= ReadByteFromIo(unWordAlignedAddress + 1) << 8;
	unData |= ReadByteFromIo(unWordAlignedAddress + 2) << 16;
	unData |= ReadByteFromIo(unWordAlignedAddress + 3) << 24;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x05:
    case 0x06:
    case 0x07:
	unData = m_Ppu->ReadWordFromBus(unWordAlignedAddress);
	break;

    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	unData |= m_aGamePakRom[unWordAlignedAddress & 0x01FFFFFF];
	unData |= m_aGamePakRom[(unWordAlignedAddress + 1) & 0x01FFFFFF] << 8;
	unData |= m_aGamePakRom[(unWordAlignedAddress + 2) & 0x01FFFFFF] << 16;
	unData |= m_aGamePakRom[(unWordAlignedAddress + 3) & 0x01FFFFFF] << 24;
	if (m_pScheduler)
	    m_pScheduler->m_ulSystemClock += 5; // No waitstates counting yet; this value should also be 8, but is currently 5 to not be so slow
	break;
    }
    return unData;
}

uint16_t Memory::ReadHalfWord(uint32_t unAddress, AccessType armAccessType) {
    uint32_t unHalfWordAlignedAddress = unAddress & ~0b1;
    uint16_t usnData = 0x00;
    switch (unAddress >> 24) {
    case 0x00:
	// BIOS
	usnData |= m_aBiosRom[unHalfWordAlignedAddress & 0x3FFF];
	usnData |= m_aBiosRom[(unHalfWordAlignedAddress + 1) & 0x3FFF] << 8;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x02:
	// Technically cannot read if greater than 0x3FFFF, fix this later
	usnData |= m_aOnBoardWorkRam[unHalfWordAlignedAddress & 0x3FFFF];
	usnData |= m_aOnBoardWorkRam[(unHalfWordAlignedAddress + 1) & 0x3FFFF] << 8;
	m_pScheduler->m_ulSystemClock += 3;
	break;

    case 0x03:
	usnData |= m_aOnChipWorkRam[unHalfWordAlignedAddress & 0x7FFF];
	usnData |= m_aOnChipWorkRam[(unHalfWordAlignedAddress + 1) & 0x7FFF] << 8;
	m_pScheduler->m_ulSystemClock++;
	break;
	// TODO: IO and SRAM

    case 0x04:
	usnData |= ReadByteFromIo(unHalfWordAlignedAddress);
	usnData |= ReadByteFromIo(unHalfWordAlignedAddress + 1) << 8;
	break;

    case 0x05:
    case 0x06:
    case 0x07:
	usnData = m_Ppu->ReadHalfFromBus(unHalfWordAlignedAddress);
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	usnData |= m_aGamePakRom[unHalfWordAlignedAddress & 0x01FFFFFF];
	usnData |= m_aGamePakRom[(unHalfWordAlignedAddress + 1) & 0x01FFFFFF] << 8;
	m_pScheduler->m_ulSystemClock += 5;
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
	m_pScheduler->m_ulSystemClock++;
	break;
    case 0x02:
	// Technically cannot read if greater than 0x3FFF, fix this later
	ubyData = m_aOnBoardWorkRam[unAddress & 0x3FFFF];
	m_pScheduler->m_ulSystemClock += 3;
	break;
    case 0x03:
	ubyData = m_aOnChipWorkRam[unAddress & 0x7FFF];
	m_pScheduler->m_ulSystemClock++;
	break;
	// TODO: IO and SRAM

    case 0x04:
	ubyData = ReadByteFromIo(unAddress);
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x05:
    case 0x06:
    case 0x07:
	ubyData = m_Ppu->ReadByteFromBus(unAddress);
	break;

    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
	ubyData = m_aGamePakRom[unAddress & 0x01FFFFFF];
	m_pScheduler->m_ulSystemClock += 5;
	break;
    }
    return ubyData;
}

void Memory::ConnectPpu(PPU* ppu) {
    m_Ppu = ppu;
}

void Memory::ConnectScheduler(Scheduler* pScheduler) {
    m_pScheduler = pScheduler;
}
