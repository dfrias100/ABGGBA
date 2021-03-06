/*+==============================================================================
  File:      GBA.h

  Summary:   Defines the GBA class and declares its methods and fields.

  Classes:   GBA

  ABGGBA: Nintendo Game Boy Advance emulator using wxWidgets and SDL2
  Copyright(C) 2022  Daniel Frias

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
==============================================================================+*/

#ifndef GBA_H
#define GBA_H

#include <fstream>
#include <cstdint>
#include <vector>

#include "../ARM7TDMI/ARM7TDMI.h"
#include "../PPU/PPU.h"
#include "../Event/Event.h"
#include "../Scheduler/Scheduler.h"

class GBA {
public:
    GBA();
    ~GBA();
    void RunUntilFrame();
    //bool LoadRom(std::ifstream& ifsRomFile);
    bool m_bLoadStateFlag = false;
    bool m_bSaveStateFlag = false;
    uint32_t* GetGraphicsArrayPointer();
private:
    Scheduler m_Scheduler;
    ARM7TDMI* m_Cpu;
    PPU* m_Ppu;
    void ConnectSchedulerToComponents();
    //void DumpState();
    //void LoadState(std::vector<uint8_t>& vbyState);
};

#endif