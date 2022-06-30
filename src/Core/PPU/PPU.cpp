#include "PPU.h"
#include "../GBA/GBA.h"

PPU::PPU() {
    std::fill(std::begin(m_aV_RAM), std::end(m_aV_RAM), 0x00);
    std::fill(std::begin(m_aOAM), std::end(m_aOAM), 0x00);
    std::fill(std::begin(m_aPaletteRAM), std::end(m_aPaletteRAM), 0x00);
    std::fill(std::begin(m_aunDisplay), std::end(m_aunDisplay), 0xFF000000);
}

void PPU::InitEvents() {
    m_evtFakeDraw.m_Callback = [&](uint64_t ulLateCycles) {
	for (size_t i = 0; i < 240 * 160; i++) {
	    uint8_t ubyColorIndex = m_aV_RAM[i];
	    uint32_t unGpuColor = 0x00;
	    uint16_t unGbaColor = m_aPaletteRAM[ubyColorIndex * 2] | m_aPaletteRAM[ubyColorIndex * 2 + 1] << 8;
	    // 0b?BBBBBgggggrrrrr

	    uint32_t unColorRed = ((float)(unGbaColor & 0x1F) / (31.0f)) * 255;
	    uint32_t unColorGreen = ((float)((unGbaColor & (0x1F << 5)) >> 5) / (31.0f)) * 255;
	    uint32_t unColorBlue = ((float)((unGbaColor & (0x1F << 10)) >> 10) / (31.0f)) * 255;

	    unColorRed = unColorRed <= 255 ? unColorRed : 255;
	    unColorGreen = unColorGreen <= 255 ? unColorGreen : 255;
	    unColorBlue = unColorBlue <= 255 ? unColorBlue : 255;

	    unGpuColor |= 0xFF000000;
	    unGpuColor |= unColorRed << 16;
	    unGpuColor |= unColorGreen << 8;
	    unGpuColor |= unColorBlue;

	    m_aunDisplay[i] = unGpuColor;
	}
	bFrameReady = true;
	m_pScheduler->ScheduleEvent(m_evtFakeDraw, 100000);
    };

    m_pScheduler->ScheduleEvent(m_evtFakeDraw, 100000);
}

uint8_t PPU::ReadByteFromBus(uint32_t unAddress) {
    uint8_t ubyData = 0x00;
    switch (unAddress >> 24) {
    case 0x05:
	ubyData = m_aPaletteRAM[unAddress & 0x3FF];
	break;

    case 0x06:
	ubyData = m_aV_RAM[unAddress & 0x17FFF];
	break;

    case 0x07:
	ubyData = m_aOAM[unAddress & 0x3FF];
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
	break;

    case 0x06:
	usnData |= m_aV_RAM[unAddress & 0x17FFF];
	usnData |= m_aV_RAM[(unAddress + 1) & 0x17FFF] << 8;
	break;

    case 0x07:
	usnData |= m_aOAM[unAddress & 0x3FF];
	usnData |= m_aOAM[(unAddress + 1) & 0x3FF] << 8;
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
	break;

    case 0x06:
	unData |= m_aV_RAM[unAddress & 0x17FFF];
	unData |= m_aV_RAM[(unAddress + 1) & 0x17FFF] << 8;
	unData |= m_aV_RAM[(unAddress + 2) & 0x17FFF] << 16;
	unData |= m_aV_RAM[(unAddress + 3) & 0x17FFF] << 24;
	break;

    case 0x07:
	unData |= m_aOAM[unAddress & 0x3FF];
	unData |= m_aOAM[(unAddress + 1) & 0x3FF] << 8;
	unData |= m_aOAM[(unAddress + 2) & 0x3FF] << 16;
	unData |= m_aOAM[(unAddress + 3) & 0x3FF] << 24;
	break;
    }
    return unData;
}

void PPU::WriteHalf(uint32_t unAddress, uint16_t usnData) {
    switch (unAddress >> 24) {
    case 0x05:
	m_aPaletteRAM[unAddress & 0x3FF] = usnData & 0xFF;
	m_aPaletteRAM[(unAddress + 1) & 0x3FF] = (usnData >> 8) & 0xFF;
	break;

    case 0x06:
	m_aV_RAM[unAddress & 0x17FFF] = usnData & 0xFF;
	m_aV_RAM[(unAddress + 1) & 0x17FFF] = (usnData >> 8) & 0xFF;
	break;

    case 0x07:
	m_aOAM[unAddress & 0x3FF] = usnData & 0xFF;
	m_aOAM[(unAddress + 1) & 0x3FF] = (usnData >> 8) & 0xFF;
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
	break;

    case 0x06:
	m_aV_RAM[unAddress & 0x17FFF]       =  unData        & 0xFF;
	m_aV_RAM[(unAddress + 1) & 0x17FFF] = (unData >>  8) & 0xFF;
	m_aV_RAM[(unAddress + 2) & 0x17FFF] = (unData >> 16) & 0xFF;
	m_aV_RAM[(unAddress + 3) & 0x17FFF] = (unData >> 24) & 0xFF;
	break;

    case 0x07:
	m_aOAM[unAddress & 0x3FF]       =  unData        & 0xFF;
	m_aOAM[(unAddress + 1) & 0x3FF] = (unData >>  8) & 0xFF;
	m_aOAM[(unAddress + 2) & 0x3FF] = (unData >> 16) & 0xFF;
	m_aOAM[(unAddress + 3) & 0x3FF] = (unData >> 24) & 0xFF;
	break;
    }
}

void PPU::ConnectScheduler(Scheduler* pScheduler) {
    m_pScheduler = pScheduler;
}

uint32_t* PPU::GetGraphicsArrayPointer() {
    return m_aunDisplay;
}
