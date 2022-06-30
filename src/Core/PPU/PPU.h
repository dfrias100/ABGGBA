#ifndef PPU_H
#define PPU_H

#include <algorithm>

#include <cstdint>

#include "../Event/Event.h"
#include "../Scheduler/Scheduler.h"

class PPU {
public:
    PPU();

    bool bFrameReady = false;
    uint8_t ReadByteFromBus(uint32_t unAddress);
    uint16_t ReadHalfFromBus(uint32_t unAddress);
    uint32_t ReadWordFromBus(uint32_t unAddress);
    
    void WriteHalf(uint32_t unAddress, uint16_t usnData);
    void WriteWord(uint32_t unAddress, uint32_t unData);
    void ConnectScheduler(Scheduler* pScheduler);
    void InitEvents();

    uint32_t* GetGraphicsArrayPointer();
private:
    Scheduler* m_pScheduler = nullptr;
    Event m_evtFakeDraw;
    uint8_t m_aV_RAM[96 * 1024];
    uint8_t m_aOAM[1024];
    uint8_t m_aPaletteRAM[1024];
    uint32_t m_aunDisplay[240 * 160];
};

#endif