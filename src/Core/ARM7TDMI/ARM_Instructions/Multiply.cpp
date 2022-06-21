/*+==============================================================================
  File:      Multiply.cpp

  Summary:   Implementation of the Multiply and Multiply Long instructions.

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::Multiply, ARM7TDMI::MultiplyLong

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

void ARM7TDMI::Multiply(uint32_t unInstruction) {
    bool bAccumulate = (unInstruction & (1 << 21)) != 0;
    bool bSetConditionCode = (unInstruction & (1 << 20)) != 0;

    uint32_t unRegisterM =  unInstruction &  0x0000F;
    uint32_t unRegisterS = (unInstruction & (0x00F00)) >>  8;
    uint32_t unRegisterN = (unInstruction & (0x0F000)) >> 12;
    uint32_t unRegisterD = (unInstruction & (0xF0000)) >> 16;

    uint32_t  unLeftOperand  = m_aRegisters[unRegisterM];
    uint32_t  unRightOperand = m_aRegisters[unRegisterS];
    uint32_t& unrDestination = m_aRegisters[unRegisterD];

    uint32_t ulResult = unLeftOperand * unRightOperand;

    if (bAccumulate) {
	ulResult += m_aRegisters[unRegisterN];
	// Idle
    }
    
    // Set condition codes
    MSC(ulResult, bSetConditionCode);

    unrDestination = ulResult;

    int32_t nM = 1;

    uint32_t unShiftingMask = 0xFF'FF'FF'00;

    while (unShiftingMask) {
	unRightOperand &= unShiftingMask;

	if (unRightOperand == 0 || unRightOperand == unShiftingMask)
	    break;

	nM++;
	unShiftingMask <<= 8;
    }

    // Idle here
}

void ARM7TDMI::MultiplyLong(uint32_t unInstruction) {
    bool bUnsigned = (unInstruction & (1 << 22)) == 0;
    bool bAccumulate = (unInstruction & (1 << 21)) != 0;
    bool bSetConditionCode = (unInstruction & (1 << 20)) != 0;

    uint32_t unRegisterM  =  unInstruction &  0x0000F;
    uint32_t unRegisterS  = (unInstruction & (0x00F00)) >> 8;
    uint32_t unRegisterLo = (unInstruction & (0x0F000)) >> 12;
    uint32_t unRegisterHi = (unInstruction & (0xF0000)) >> 16;

    uint64_t ulLeftOperand  = m_aRegisters[unRegisterM];
    uint64_t ulRightOperand = m_aRegisters[unRegisterS];

    uint64_t ulDestinationHiLo = ((uint64_t)m_aRegisters[unRegisterHi] << 32ULL) | m_aRegisters[unRegisterLo];
    uint64_t ulAccumulateAmount = ulDestinationHiLo;

    int32_t nM = 1;

    auto lamCycleCountingFunction = [&nM](bool bSigned, uint32_t unRegisterMCopy) {
	uint32_t unShiftingMask = 0xFF'FF'FF'00;
	while (unShiftingMask) {
	    unRegisterMCopy &= unShiftingMask;

	    if (unRegisterMCopy == 0 || (bSigned && (unRegisterMCopy == unShiftingMask)))
		return;

	    nM++;
	    unShiftingMask <<= 8;
	}
    };

    auto lamSignExtend = [](uint32_t unSignedNum) {
	uint64_t ulReturnVal = unSignedNum;
	if (ulReturnVal & 0x8000'0000)
	    ulReturnVal |= 0xFFFF'FFFF'0000'0000ULL;
	return ulReturnVal;
    };

    lamCycleCountingFunction(!bUnsigned, ulRightOperand);

    if (bUnsigned) {
	ulDestinationHiLo = ulLeftOperand * ulRightOperand;
    } else {
	ulLeftOperand = lamSignExtend(ulLeftOperand);
	ulRightOperand = lamSignExtend(ulRightOperand);

	ulDestinationHiLo = ulLeftOperand * ulRightOperand;
    }

    if (bAccumulate) {
	ulDestinationHiLo += ulAccumulateAmount;
	// Idle
    }

    if (bSetConditionCode) {
	m_CPSR.N = (ulDestinationHiLo & 0x8000'0000'0000'0000ULL) != 0;
	m_CPSR.Z = ulDestinationHiLo == 0;
    }

    // Idle again

    // Idle again for M cycles

    m_aRegisters[unRegisterHi] = ulDestinationHiLo >> 32ULL;
    m_aRegisters[unRegisterLo] = ulDestinationHiLo & 0xFFFF'FFFF;
}
