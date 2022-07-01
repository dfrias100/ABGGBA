/*+==============================================================================
  File:      DataTransfer.cpp

  Summary:   Instructions of the ARM7TDMI that correspond to the transfer
	     of data between registers and memory

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::SingleDataSwap,
	     ARM7TDMI::HalfWordDataTransfer,
	     ARM7TDMI::SingleDataTransfer,
	     ARM7TDMI::BlockDataTransfer

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

void ARM7TDMI::SingleDataSwap(uint32_t unInstruction) {
    bool bByteQuantity = (unInstruction & (1 << 22));

    uint32_t unRegisterN = (unInstruction & (0xF << 16)) >> 16;
    uint32_t unRegisterD = (unInstruction & (0xF << 12)) >> 12;
    uint32_t unRegisterM =  unInstruction &  0xF;

    uint32_t  unBaseAddress         = m_aRegisters[unRegisterN];
    uint32_t& unDestinationRegister = m_aRegisters[unRegisterD];
    uint32_t  unSourceRegister      = m_aRegisters[unRegisterM];

    uint32_t unSourceData;

    if (bByteQuantity) {
	unSourceData = m_Mmu.ReadByte(unBaseAddress, AccessType::NonSequential);
	m_Mmu.WriteByte(unBaseAddress, unSourceRegister, AccessType::NonSequential);
    } else {
	unSourceData = m_Mmu.ReadWord(unBaseAddress, AccessType::NonSequential);
	m_Mmu.WriteWord(unBaseAddress, unSourceRegister, AccessType::NonSequential);
	if ((unBaseAddress = (unBaseAddress & 0x3) << 3)) {
	    // Read is misaligned
	    unSourceData = ROR(unSourceData, unBaseAddress, false, false);
	}
    }

    unDestinationRegister = unSourceData;

    // Idle
    m_pScheduler->m_ulSystemClock++;
}

void ARM7TDMI::HalfwordDataTransfer(uint32_t unInstruction) {
    bool bPreIndexing = (unInstruction & (1 << 24)) != 0;
    bool bUpOffset    = (unInstruction & (1 << 23)) != 0;
    bool bImmOffset   = (unInstruction & (1 << 22)) != 0;
    bool bWriteBack   = (unInstruction & (1 << 21)) != 0;
    bool bLoad        = (unInstruction & (1 << 20)) != 0;

    uint32_t unDataType = (unInstruction & (0b0110 << 4)) >> 5;

    uint32_t unRegisterN = (unInstruction & (0xF0000)) >> 16;
    uint32_t unRegisterD = (unInstruction & (0x0F000)) >> 12;
    uint32_t unRegisterM = (unInstruction & (0x0000F));

    uint32_t unBaseAddress = m_aRegisters[unRegisterN];
    uint32_t& unSrcOrDest  = m_aRegisters[unRegisterD];

    uint32_t unOffset;

    if (bImmOffset) {
	unOffset = ((unInstruction & 0xF00) >> 4) | unRegisterM;
    } else {
	unOffset = m_aRegisters[unRegisterM];
    }

    if (!bUpOffset)
	unOffset = ~unOffset + 1; // Take the two's complement

    if (bPreIndexing)
	unBaseAddress = unBaseAddress + unOffset;

    uint32_t unData;

    if (bLoad) {
	// LDRH, LDRSB, LDRSH
	auto lamSignExtend = [](uint32_t unNum, bool bIsHalf) {
	    if (bIsHalf) {
		if (unNum & 0x80'00)
		    unNum |= 0xFF'FF'00'00;
	    } else {
		if (unNum & 0x80)
		    unNum |= 0xFF'FF'FF'00;
	    }
	    return unNum;
	};

	bool bIsLDRH = unDataType == 0b01;
	bool bIsLDRSH = unDataType == 0b11;

	if (unDataType & 0x1) {
	    unData = m_Mmu.ReadHalfWord(unBaseAddress, AccessType::NonSequential);
	    if (unBaseAddress & 0x1) {
		// Possible misalignment
		if (bIsLDRH) {
		    unData = ROR(unData, 8, false, false);
		} else if (bIsLDRSH) {
		    unData = lamSignExtend(unData >> 8, false);
		}
	    }
	} else {
	    unData = m_Mmu.ReadByte(unBaseAddress, AccessType::NonSequential);
	}

	if (unDataType & 0x2)
	    unData = lamSignExtend(unData, unDataType & 0x1);

	// Idle
	m_pScheduler->m_ulSystemClock++;

	unSrcOrDest = unData;

	if (unRegisterD == 15)
	    FlushPipelineARM();
    } else {
	// STRHs
	if (unRegisterD == 15) {
	    m_Mmu.WriteHalfWord(unBaseAddress, unSrcOrDest + 4, AccessType::NonSequential);
	} else
	    m_Mmu.WriteHalfWord(unBaseAddress, unSrcOrDest, AccessType::NonSequential);
    }

    if ((bWriteBack || !bPreIndexing) && (!bLoad || unRegisterN != unRegisterD)) {
	if (!bPreIndexing)
	    unBaseAddress = unBaseAddress + unOffset;

	m_aRegisters[unRegisterN] = unBaseAddress;
    }
}

void ARM7TDMI::SingleDataTransfer(uint32_t unInstruction) {
    bool bIsRegister   = (unInstruction & (1 << 25)) != 0;
    bool bPreIndexing  = (unInstruction & (1 << 24)) != 0;
    bool bUpOffset     = (unInstruction & (1 << 23)) != 0;
    bool bByteQuantity = (unInstruction & (1 << 22)) != 0;
    bool bWriteBack    = (unInstruction & (1 << 21)) != 0;
    bool bLoad         = (unInstruction & (1 << 20)) != 0;

    uint32_t unRegisterN = (unInstruction & 0xF0000) >> 16;
    uint32_t unRegisterD = (unInstruction & 0x0F000) >> 12;
    uint32_t unOffset    =  unInstruction & 0x00FFF;

    uint32_t  unBaseAddress = m_aRegisters[unRegisterN];
    uint32_t  unBaseAddrCpy = unBaseAddress;
    uint32_t& unSrcOrDest   = m_aRegisters[unRegisterD];

    uint32_t unOffsetVal;

    if (bIsRegister) {
	// See data processing
	uint8_t  ubyShiftType = (unOffset & 0b0110'0000) >> 5;
	uint16_t usnShiftAmount = (unOffset & 0xF80) >> 7;
	uint32_t unOffsetRegister = m_aRegisters[unOffset & 0xF];

	switch (ubyShiftType) {
	case 0x00: unOffsetVal = LSL(unOffsetRegister, usnShiftAmount, false); break;
	case 0x01: unOffsetVal = LSR(unOffsetRegister, usnShiftAmount, false, true); break;
	case 0x02: unOffsetVal = ASR(unOffsetRegister, usnShiftAmount, false, true); break;
	case 0x03: unOffsetVal = ROR(unOffsetRegister, usnShiftAmount, false, true); break;
	}
    } else {
	unOffsetVal = unOffset;
    }

    if (!bUpOffset)
	unOffsetVal = ~unOffsetVal + 1;

    if (bPreIndexing)
	unBaseAddress = unBaseAddress + unOffsetVal;

    if (bLoad) {
	uint32_t unLoadedData;

	if (bByteQuantity)
	    unLoadedData = m_Mmu.ReadByte(unBaseAddress, AccessType::NonSequential);
	else {
	    unLoadedData = m_Mmu.ReadWord(unBaseAddress, AccessType::NonSequential);
	    if ((unBaseAddrCpy = (unBaseAddress & 0x3) << 3)) {
		// Read is misaligned
		unLoadedData = ROR(unLoadedData, unBaseAddrCpy, false, false);
	    }
	}

	unSrcOrDest = unLoadedData;

	// Idle here
	m_pScheduler->m_ulSystemClock++;

	if (unRegisterD == 15)
	    FlushPipelineARM();
    } else {
	uint32_t unDataToWrite = unSrcOrDest + (unRegisterD == 15 ? 4 : 0);

	if (bByteQuantity)
	    m_Mmu.WriteByte(unBaseAddress, unDataToWrite, AccessType::NonSequential);
	else {
	    m_Mmu.WriteWord(unBaseAddress, unDataToWrite, AccessType::NonSequential);
	}
    }

    if ((bWriteBack || !bPreIndexing) && (unRegisterD != unRegisterN || !bLoad)) {
	if (!bPreIndexing)
	    unBaseAddress = unBaseAddress + unOffsetVal;

	m_aRegisters[unRegisterN] = unBaseAddress;
    }
}

void ARM7TDMI::BlockDataTransfer(uint32_t unInstruction) {
    bool bPreIndexing = (unInstruction & (1 << 24)) != 0;
    bool bUpOffset    = (unInstruction & (1 << 23)) != 0;
    bool bPsrForceUsr = (unInstruction & (1 << 22)) != 0;
    bool bWriteBack   = (unInstruction & (1 << 21)) != 0;
    bool bLoad        = (unInstruction & (1 << 20)) != 0;

    uint32_t unRegisterN = (unInstruction & 0xF0000) >> 16;
    uint32_t unBaseAddress = m_aRegisters[unRegisterN];
    uint32_t unBaseAddressOrig = unBaseAddress;

    uint16_t usnRegisterList = unInstruction & 0xFFFF;

    CPU_Mode armCurrMode;
    if (bPsrForceUsr) {
	if (!(usnRegisterList & (1 << 15)) || !bLoad) {
	    armCurrMode = static_cast<CPU_Mode>(m_CPSR.Mode);
	    SwitchMode(CPU_Mode::USR);
	}
    }

    if (usnRegisterList != 0) {
	auto lamBitCount = [&]() {
	    uint32_t unRegListCpy = usnRegisterList;
	    uint32_t unBitCount = 0;
	    while (unRegListCpy) {
		if (unRegListCpy & 0x1)
		    unBitCount++;
		unRegListCpy >>= 1;
	    }
	    return unBitCount;
	};

	uint32_t unRegCount = lamBitCount();

	if (!bUpOffset)
	    unBaseAddress -= unRegCount * 4;

	bool bAltPreIndex = bPreIndexing ^ !bUpOffset;

	AccessType armAccessType = AccessType::NonSequential;


	auto lamCountTrailingZeros = [&]() {
	    uint32_t unZeroCount = 0;
	    uint32_t unRegisterListCpy = usnRegisterList;
	    while (unRegisterListCpy) {
		if ((unRegisterListCpy & 0x1) == 0)
		    unZeroCount++;
		else
		    break;
		unRegisterListCpy >>= 1;
	    }
	    return unZeroCount;
	};

	if (!bLoad && (lamCountTrailingZeros() == unRegisterN)) {
	    if (bAltPreIndex)
		m_Mmu.WriteWord(unBaseAddress + 4, unBaseAddressOrig, armAccessType);
	    else
		m_Mmu.WriteWord(unBaseAddress, unBaseAddressOrig, armAccessType);

	    unBaseAddress += 4;
	    unBaseAddressOrig += bUpOffset ? 4 : -4;

	    usnRegisterList &= ~(1 << unRegisterN);
	    unRegCount--;
	    armAccessType = AccessType::Sequential;
	}

	if (bWriteBack)
	    m_aRegisters[unRegisterN] = bUpOffset ? (unBaseAddressOrig + unRegCount * 4) :
						    (unBaseAddressOrig - unRegCount * 4);

	if (bLoad) {
	    for (uint32_t i = 0; i < 16; i++) {
		if (usnRegisterList & (1 << i)) {
		    if (bAltPreIndex)
			unBaseAddress += 4;

		    m_aRegisters[i] = m_Mmu.ReadWord(unBaseAddress, armAccessType);
		    armAccessType = AccessType::Sequential;

		    if (!bAltPreIndex)
			unBaseAddress += 4;
		}
	    }
	    
	    // Idle cycle
	    m_pScheduler->m_ulSystemClock++;
	} else {
	    uint32_t unPcVal = m_PC;

	    m_PC += 4;

	    for (uint32_t i = 0; i < 16; i++) {
		if (usnRegisterList & (1 << i)) {
		    if (bAltPreIndex)
			unBaseAddress += 4;

		    m_Mmu.WriteWord(unBaseAddress, m_aRegisters[i], armAccessType);
		    armAccessType = AccessType::Sequential;

		    if (!bAltPreIndex)
			unBaseAddress += 4;
		}
	    }

	    m_PC = unPcVal;
	}
    } else {
	// How did we get here?
	if (bLoad) {
	    m_PC = m_Mmu.ReadWord(unBaseAddress, AccessType::NonSequential);
	    FlushPipelineARM();
	} else {
	    if (bUpOffset)
		m_Mmu.WriteWord(unBaseAddress + (bPreIndexing ? 4 : 0), m_PC + 4, AccessType::NonSequential);
	    else
		m_Mmu.WriteWord(unBaseAddress - (bPreIndexing ? 0x40 : 0x3C), m_PC + 4, AccessType::NonSequential);
	}
	if (bWriteBack)
	    m_aRegisters[unRegisterN] = bUpOffset ? (unBaseAddress + 0x40) : (unBaseAddress - 0x40);
    }

    if (bPsrForceUsr) {
	if (!(usnRegisterList & (1 << 15)) || !bLoad)
	    // STM or LDM with S bit set, without R15
	    SwitchMode(armCurrMode);
	else {
	    // LDM with R15 with S bit set
	    PSR armSpsr = m_SPSR;
	    SwitchMode(static_cast<CPU_Mode>(armSpsr.Mode));
	    m_CPSR = armSpsr;
	    if (m_CPSR.T) {
		m_CpuExecutionState |= static_cast<uint8_t>(ExecutionState::THUMB);
		FlushPipelineTHUMB();
	    } else {
		FlushPipelineARM();
	    }
	}
    } else if ((usnRegisterList & (1 << 15)) && bLoad) {
	// LDM with R15 but S bit was not set
	FlushPipelineARM();
    }
}