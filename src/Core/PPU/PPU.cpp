#include "PPU.h"
#include "../GBA/GBA.h"

PPU::PPU() {
    std::fill(std::begin(m_aV_RAM), std::end(m_aV_RAM), 0x00);
    std::fill(std::begin(m_aOAM), std::end(m_aOAM), 0x00);
    std::fill(std::begin(m_aPaletteRAM), std::end(m_aPaletteRAM), 0x00);
    std::fill(std::begin(m_aunDisplay), std::end(m_aunDisplay), 0xFF000000);
    std::memset(&m_ppuDispcnt,  0, sizeof(DISPCNT));
    std::memset(&m_ppuVcount,   0, sizeof(VCOUNT));
    std::memset(&m_ppuDispstat, 0, sizeof(DISPSTAT));
}

void PPU::InitEvents() {
    // TODO: Handle IRQ, DMA, etc. etc. etc
    m_evtHblank.m_Callback = [&](uint64_t ulCyclesLate) {
	m_ppuDispstat.ppuHblank = true;

	if (m_ppuVcount.ppuScanline < 160) {
	    DrawScanline();
	}

	m_pScheduler->ScheduleEvent(m_evtEndHblank, 226 - ulCyclesLate);
    };

    m_evtEndHblank.m_Callback = [&](uint64_t ulCyclesLate) {
	m_ppuVcount.ppuScanline = (m_ppuVcount.ppuScanline + 1) % 228;
	m_ppuDispstat.ppuHblank = false;
	m_ppuDispstat.ppuVblank = m_ppuVcount.ppuScanline >= 160 && m_ppuVcount.ppuScanline < 227;

	m_ppuDispstat.ppuVcountMatch = m_ppuVcount.ppuScanline == m_ppuDispstat.ppuLyc;

	if (m_ppuVcount.ppuScanline == 160) {
	    bFrameReady = true;
	}

	m_pScheduler->ScheduleEvent(m_evtHblank, 1006 - ulCyclesLate);
    };

    m_pScheduler->ScheduleEvent(m_evtHblank, 1006);
}

void PPU::WriteByteToRegister(PPU::IoRegister ppuReg, uint8_t ubyData, uint32_t unByteIndex) {
    unByteIndex <<= 3;
    switch (ppuReg) {
    case PPU::IoRegister::IO_DISPCNT:
	{
	    m_ppuDispcnt.usnDispcntPacked &=  ~(0xFF << unByteIndex);
	    m_ppuDispcnt.usnDispcntPacked |= ubyData << unByteIndex;
	}
	break;
    case PPU::IoRegister::IO_DISPSTAT:
	{
	    uint8_t ubyReadOnlyRegs = m_ppuDispstat.usnDispstatPacked & 0b111;
	    m_ppuDispstat.usnDispstatPacked &=  ~(0xFF << unByteIndex);
	    m_ppuDispstat.usnDispstatPacked |= ubyData << unByteIndex;

	    m_ppuDispstat.usnDispstatPacked &= ~0b111;
	    m_ppuDispstat.usnDispstatPacked |= ubyReadOnlyRegs;
	}
	break;
    }
}

uint8_t PPU::ReadByteFromRegister(PPU::IoRegister ppuReg, uint32_t unByteIndex) {
    uint32_t unData = 0x00;
    unByteIndex <<= 3;
    switch (ppuReg) {
    case PPU::IoRegister::IO_DISPCNT:
	{
	    unData = (m_ppuDispcnt.usnDispcntPacked & (0xFF << unByteIndex)) >> unByteIndex;
	}
	break;
    case PPU::IoRegister::IO_DISPSTAT:
	{
	    unData = (m_ppuDispstat.usnDispstatPacked & (0xFF << unByteIndex)) >> unByteIndex;
	}
	break;
    case PPU::IoRegister::IO_VCOUNT:
	{
	    unData = (m_ppuVcount.usnVcountPacked & (0xFF << unByteIndex)) >> unByteIndex;
	}
	break;
    }
    return unData;
}

void PPU::DrawScanline() {
    std::fill(std::begin(m_aBgPixelScanline), std::end(m_aBgPixelScanline), 0);

    switch (m_ppuDispcnt.ppuBgMode) {
    case 4: 
	DrawBG_Mode4();
	OutputPixels();
	break;
    default:
	std::copy(std::begin(m_aBgPixelScanline), std::end(m_aBgPixelScanline), &m_aunDisplay[m_ppuVcount.ppuScanline * 240]);
	break;
    }
}

void PPU::DrawBG_Mode4() {
    // TODO: Handle the affine transform
    for (size_t x = 0; x < 240; x++) {
	uint32_t addr = m_ppuDispcnt.ppuFrameSelect * 0xA000 + m_ppuVcount.ppuScanline * 240 + x;
	uint8_t  ubyColorIndex = m_aV_RAM[addr];
	uint16_t unGbaColor    = m_aPaletteRAM[ubyColorIndex * 2] | m_aPaletteRAM[ubyColorIndex * 2 + 1] << 8;
	m_aBgPixelScanline[x]  = m_aGpuColorLut[unGbaColor & ~0x8000];
    }
}

void PPU::OutputPixels() {
    std::copy(std::begin(m_aBgPixelScanline), std::end(m_aBgPixelScanline), &m_aunDisplay[m_ppuVcount.ppuScanline * 240]);
}

uint8_t PPU::ReadByteFromBus(uint32_t unAddress) {
    uint8_t ubyData = 0x00;
    switch (unAddress >> 24) {
    case 0x05:
	ubyData = m_aPaletteRAM[unAddress & 0x3FF];
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x06:
	ubyData = m_aV_RAM[unAddress & 0x17FFF];
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x07:
	ubyData = m_aOAM[unAddress & 0x3FF];
	m_pScheduler->m_ulSystemClock++;
	break;
    }
    return ubyData;
}

uint16_t PPU::ReadHalfFromBus(uint32_t unAddress) {
    uint16_t usnData = 0x00;
    switch (unAddress >> 24) {
    case 0x05:
	usnData |= m_aPaletteRAM[unAddress & 0x3FF];
	usnData |= m_aPaletteRAM[(unAddress + 1) & 0x3FF] << 8;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x06:
	usnData |= m_aV_RAM[unAddress & 0x17FFF];
	usnData |= m_aV_RAM[(unAddress + 1) & 0x17FFF] << 8;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x07:
	usnData |= m_aOAM[unAddress & 0x3FF];
	usnData |= m_aOAM[(unAddress + 1) & 0x3FF] << 8;
	m_pScheduler->m_ulSystemClock++;
	break;
    }
    return usnData;
}

uint32_t PPU::ReadWordFromBus(uint32_t unAddress) {
    uint16_t unData = 0x00;
    switch (unAddress >> 24) {
    case 0x05:
	unData |= m_aPaletteRAM[unAddress & 0x3FF];
	unData |= m_aPaletteRAM[(unAddress + 1) & 0x3FF] << 8;
	unData |= m_aPaletteRAM[(unAddress + 2) & 0x3FF] << 16;
	unData |= m_aPaletteRAM[(unAddress + 3) & 0x3FF] << 24;
	m_pScheduler->m_ulSystemClock += 2;
	break;

    case 0x06:
	unData |= m_aV_RAM[unAddress & 0x17FFF];
	unData |= m_aV_RAM[(unAddress + 1) & 0x17FFF] << 8;
	unData |= m_aV_RAM[(unAddress + 2) & 0x17FFF] << 16;
	unData |= m_aV_RAM[(unAddress + 3) & 0x17FFF] << 24;
	m_pScheduler->m_ulSystemClock += 2;
	break;

    case 0x07:
	unData |= m_aOAM[unAddress & 0x3FF];
	unData |= m_aOAM[(unAddress + 1) & 0x3FF] << 8;
	unData |= m_aOAM[(unAddress + 2) & 0x3FF] << 16;
	unData |= m_aOAM[(unAddress + 3) & 0x3FF] << 24;
	m_pScheduler->m_ulSystemClock++;
	break;
    }
    return unData;
}

void PPU::WriteHalf(uint32_t unAddress, uint16_t usnData) {
    switch (unAddress >> 24) {
    case 0x05:
	m_aPaletteRAM[unAddress & 0x3FF] = usnData & 0xFF;
	m_aPaletteRAM[(unAddress + 1) & 0x3FF] = (usnData >> 8) & 0xFF;
	m_pScheduler->m_ulSystemClock++;
	break;

    case 0x06: 
	{
	    uint32_t unNewAddress = unAddress & ((unAddress & 0x1'0000) ? 0x17FFF : 0x0FFFF);
	    m_aV_RAM[unNewAddress] = usnData & 0xFF;
	    m_aV_RAM[unNewAddress + 1] = (usnData >> 8) & 0xFF;
	    m_pScheduler->m_ulSystemClock++;
	}
	break;

    case 0x07:
	m_aOAM[unAddress & 0x3FF] = usnData & 0xFF;
	m_aOAM[(unAddress + 1) & 0x3FF] = (usnData >> 8) & 0xFF;
	m_pScheduler->m_ulSystemClock++;
	break;
    }
}

void PPU::WriteWord(uint32_t unAddress, uint32_t unData) {
    switch (unAddress >> 24) {
    case 0x05:
	m_aPaletteRAM[unAddress & 0x3FF]       =  unData        & 0xFF;
	m_aPaletteRAM[(unAddress + 1) & 0x3FF] = (unData >>  8) & 0xFF;
	m_aPaletteRAM[(unAddress + 2) & 0x3FF] = (unData >> 16) & 0xFF;
	m_aPaletteRAM[(unAddress + 3) & 0x3FF] = (unData >> 24) & 0xFF;
	m_pScheduler->m_ulSystemClock += 2;
	break;

    case 0x06:
	{
	    uint32_t unNewAddress = unAddress & ((unAddress & 0x1'0000) ? 0x17FFF : 0x0FFFF);
	    m_aV_RAM[unNewAddress] =  unData        & 0xFF;
	    m_aV_RAM[unNewAddress + 1] = (unData >>  8) & 0xFF;
	    m_aV_RAM[unNewAddress + 2] = (unData >> 16) & 0xFF;
	    m_aV_RAM[unNewAddress + 3] = (unData >> 24) & 0xFF;
	    m_pScheduler->m_ulSystemClock += 2;
	}
	break;

    case 0x07:
	m_aOAM[unAddress & 0x3FF]       =  unData        & 0xFF;
	m_aOAM[(unAddress + 1) & 0x3FF] = (unData >>  8) & 0xFF;
	m_aOAM[(unAddress + 2) & 0x3FF] = (unData >> 16) & 0xFF;
	m_aOAM[(unAddress + 3) & 0x3FF] = (unData >> 24) & 0xFF;
	m_pScheduler->m_ulSystemClock++;
	break;
    }
}

void PPU::ConnectScheduler(Scheduler* pScheduler) {
    m_pScheduler = pScheduler;
}

uint32_t* PPU::GetGraphicsArrayPointer() {
    return m_aunDisplay;
}
