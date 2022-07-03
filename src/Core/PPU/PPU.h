#ifndef PPU_H
#define PPU_H

#include <algorithm>

#include <cstdint>

#include "../Event/Event.h"
#include "../Scheduler/Scheduler.h"

#include "OAM_Entry.inl"
#include "PPU_IO.inl"

class PPU {
public:
    static enum IoRegister {
	IO_DISPCNT   = 0x04000000,
	GreenSwap    = 0x04000002,
	IO_DISPSTAT  = 0x04000004,
	IO_VCOUNT    = 0x04000006
    };

    PPU();

    bool bFrameReady = false;
    uint8_t ReadByteFromBus(uint32_t unAddress);
    uint16_t ReadHalfFromBus(uint32_t unAddress);
    uint32_t ReadWordFromBus(uint32_t unAddress);

    void WriteHalf(uint32_t unAddress, uint16_t usnData);
    void WriteWord(uint32_t unAddress, uint32_t unData);
    void ConnectScheduler(Scheduler* pScheduler);
    void InitEvents();

    void WriteByteToRegister(PPU::IoRegister ppuReg, uint8_t ubyData, uint32_t unByteIndex);
    uint8_t ReadByteFromRegister(PPU::IoRegister ppuReg, uint32_t unByteIndex);

    void DrawScanline();
    void DrawBG_Mode4();

    void OutputPixels();

    uint32_t* GetGraphicsArrayPointer();
private:
    Scheduler* m_pScheduler = nullptr;

    Event m_evtFakeDraw;
    Event m_evtHblank;
    Event m_evtEndHblank;

    uint8_t m_aV_RAM[96 * 1024];
    uint8_t m_aOAM[1024];
    uint8_t m_aPaletteRAM[1024];
    uint32_t m_aunDisplay[240 * 160];

    DISPCNT  m_ppuDispcnt;
    DISPSTAT m_ppuDispstat;
    VCOUNT   m_ppuVcount;

    uint32_t m_aBgPixelScanline[240];

    #include "ColorLookup.inl"
};

#endif