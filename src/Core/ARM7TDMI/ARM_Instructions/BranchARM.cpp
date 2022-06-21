/*+==============================================================================
  File:      DataProcessing.cpp

  Summary:   Instructions of the ARM7TDMI that correspond to program
	     branching

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::BranchAndExchange, ARM7TDMI::Branch

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


#include "../ARM7TDMI.h"

void ARM7TDMI::BranchAndExchange(uint32_t unInstruction) {
    uint32_t unRegisterN = unInstruction & 0xF;
    uint32_t unRegisterVal = m_aRegisters[unRegisterN];

    m_PC = unRegisterVal;

    if (unRegisterVal & 0x1) {
	m_CPSR.T = 1;
	m_CpuExecutionState |= static_cast<uint8_t>(ExecutionState::THUMB);
	FlushPipelineTHUMB();
    } else {
	m_CPSR.T = 0;
	m_CpuExecutionState &= ~static_cast<uint8_t>(ExecutionState::THUMB);
	FlushPipelineARM();
    }
}

void ARM7TDMI::Branch(uint32_t unInstruction) {
    bool bLink = (unInstruction & (1 << 24)) != 0;

    uint32_t unOffset = unInstruction & 0x00'FF'FF'FF;

    if (unOffset & 0x00'80'00'00)
	unOffset |= 0xFF'00'00'00;

    unOffset <<= 2;

    if (bLink)
	m_LR = m_PC - 4;

    m_PC += unOffset;
    FlushPipelineARM();
}