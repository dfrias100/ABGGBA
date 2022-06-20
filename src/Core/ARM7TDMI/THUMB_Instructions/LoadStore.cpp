#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::PCRelLoad(uint16_t usnInstruction) {
    uint16_t usnRegisterD = (usnInstruction & 0b111'0000'0000) >> 8;
    uint32_t unOffset = (usnInstruction & 0xFF) << 2;

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];
    uint32_t  unAddrToLoad = m_PC + unOffset;

    unDestinationRegister = m_Mmu.ReadWord(unAddrToLoad, AccessType::NonSequential);

    // Idle cycle
}

void ARM7TDMI::LoadOrStoreWithRegOff(uint16_t usnInstruction) {
    uint16_t usnRegisterO = (usnInstruction & 0b111'000'000) >> 6;
    uint16_t usnRegisterB = (usnInstruction & 0b000'111'000) >> 3;
    uint16_t usnRegisterD = (usnInstruction & 0b000'000'111);

    uint16_t  usnOpcode = (usnInstruction & 0b1100'0000'0000) >> 10;
    uint32_t  unAddress = m_aRegisters[usnRegisterB] + m_aRegisters[usnRegisterO];
    uint32_t& unSrcOrDest = m_aRegisters[usnRegisterD];

    switch (usnOpcode) {
	// STR
    case 0: m_Mmu.WriteWord(unAddress, unSrcOrDest, AccessType::NonSequential); break;
	// STRB
    case 1: m_Mmu.WriteByte(unAddress, unSrcOrDest, AccessType::NonSequential); break;
	// LDR
    case 2: 
	unSrcOrDest = m_Mmu.ReadWord(unAddress, AccessType::NonSequential); 
	if ((unAddress = (unAddress & 0x3) << 3)) {
	    // Read is misaligned
	    unSrcOrDest = ROR(unSrcOrDest, unAddress, false, false);
	}   
	break;
	// LDRB
    case 3: unSrcOrDest = m_Mmu.ReadByte(unAddress, AccessType::NonSequential); break;
    }
    
    // TODO: handle idle on ldr
}

void ARM7TDMI::LoadOrStoreSignExtendedByteOrHalfWord(uint16_t usnInstruction) {
    uint16_t usnRegisterO = (usnInstruction & 0b111'000'000) >> 6;
    uint16_t usnRegisterB = (usnInstruction & 0b000'111'000) >> 3;
    uint16_t usnRegisterD = (usnInstruction & 0b000'000'111);

    uint16_t  usnOpcode = (usnInstruction & 0b1100'0000'0000) >> 10;
    usnOpcode |= ((usnOpcode & 0x1) << 2);
    usnOpcode >>= 1;
    uint32_t  unAddress = m_aRegisters[usnRegisterB] + m_aRegisters[usnRegisterO];
    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];

    switch (usnOpcode) {
    case 0: // STRH
	m_Mmu.WriteHalfWord(unAddress, unDestinationRegister, AccessType::NonSequential); 
	break;	
    case 1: // LDRH
	unDestinationRegister = m_Mmu.ReadHalfWord(unAddress, AccessType::NonSequential);
	if (unAddress & 0x1) {
	    // Read is misaligned
	    unDestinationRegister = ROR(unDestinationRegister, 8, false, false);
	}
	break;
    case 2: // LDSB
	unDestinationRegister = m_Mmu.ReadByte(unAddress, AccessType::NonSequential);
	if (unDestinationRegister & 0x80) {
	    unDestinationRegister |= 0xFF'FF'FF'00;
	}
	break;
    case 3: // LDSH
	unDestinationRegister = m_Mmu.ReadHalfWord(unAddress, AccessType::NonSequential);
	if (unAddress & 0x1) {
	    unDestinationRegister >>= 8;
	    if (unDestinationRegister & 0x80) {
		unDestinationRegister |= 0xFF'FF'FF'00;
	    }
	} else {
	    if (unDestinationRegister & 0x8000) {
		unDestinationRegister |= 0xFF'FF'00'00;
	    }
	}
	break;
    }

    // Idle on loads
}

void ARM7TDMI::LoadOrStoreWithImmOff(uint16_t usnInstruction) {
    uint16_t usnOpcode    = (usnInstruction & 0b0001'1000'0000'0000) >> 11;
    usnOpcode |= ((usnOpcode & 0x1) << 2);
    usnOpcode >>= 1;
    uint16_t usnOffset    = (usnInstruction & 0b11111'000'000) >> 6;
    uint16_t usnRegisterB = (usnInstruction & 0b00000'111'000) >> 3;
    uint16_t usnRegisterD = (usnInstruction & 0b00000'000'111);

    uint32_t& unSrcOrDest = m_aRegisters[usnRegisterD];
    uint32_t  unAddress = m_aRegisters[usnRegisterB];

    if ((usnOpcode & 0x1) == 0)
	usnOffset <<= 2;

    unAddress += usnOffset;

    switch (usnOpcode) {
    case 0: m_Mmu.WriteWord(unAddress, unSrcOrDest, AccessType::NonSequential); break;
    case 1: m_Mmu.WriteByte(unAddress, unSrcOrDest, AccessType::NonSequential); break;
    case 2: 
	unSrcOrDest = m_Mmu.ReadWord(unAddress, AccessType::NonSequential);
	if ((unAddress = (unAddress & 0x3) << 3)) {
	    // Read is misaligned
	    unSrcOrDest = ROR(unSrcOrDest, unAddress, false, false);
	}
	break;
    case 3:
	unSrcOrDest = m_Mmu.ReadByte(unAddress, AccessType::NonSequential);
	break;
    }

    // Idle on loads
}

void ARM7TDMI::LoadOrStoreHalfword(uint16_t usnInstruction) {
    bool bLoad = (usnInstruction & 0b0000'1000'0000'0000) != 0;
    uint16_t usnOffset    = (usnInstruction & 0b11111'000'000) >> 6;
    uint16_t usnRegisterB = (usnInstruction & 0b00000'111'000) >> 3;
    uint16_t usnRegisterD = (usnInstruction & 0b00000'000'111);

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];
    uint32_t  unAddress = m_aRegisters[usnRegisterB] + (usnOffset << 1);

    if (bLoad) {
	unDestinationRegister = m_Mmu.ReadHalfWord(unAddress, AccessType::NonSequential);
    } else {
	m_Mmu.WriteWord(unAddress, unDestinationRegister, AccessType::NonSequential);
    }
}

void ARM7TDMI::SPRelLoadOrStore(uint16_t usnInstruction) {
    bool bLoad = (usnInstruction & 0b0000'1000'0000'0000) != 0;
    uint16_t usnRegisterD = (usnInstruction & 0b111'0000'0000) >> 8;
    uint32_t unOffset = (usnInstruction & 0xFF) << 2;

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];
    uint32_t  unAddress = m_SP + unOffset;

    if (bLoad)
	unDestinationRegister = m_Mmu.ReadWord(unAddress, AccessType::NonSequential);
    else
	m_Mmu.WriteWord(unAddress, unDestinationRegister, AccessType::NonSequential);

    // Idle cycle if load
}

void ARM7TDMI::LoadAddress(uint16_t usnInstruction) {
    bool bSp = (usnInstruction & 0b0000'1000'0000'0000) != 0;
    uint16_t usnRegisterD = (usnInstruction & 0b111'0000'0000) >> 8;
    uint32_t unOffset = (usnInstruction & 0xFF) << 2;

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];

    if (bSp)
	unDestinationRegister = m_SP + unOffset;
    else
	unDestinationRegister = (m_PC & ~0x2) + unOffset;

    // Idle cycle 
}

void ARM7TDMI::MultipleLoadOrStore(uint16_t usnInstruction) {
    bool bLoad = (usnInstruction & 0x800) >> 11;
    bool bWriteBack = true;

    uint16_t usnRegisterB = (usnInstruction & 0b111'0000'0000) >> 8;

    uint16_t ubyRegisterList = usnInstruction & 0xFF;
    uint32_t unAddress = m_aRegisters[usnRegisterB];
    uint32_t unBaseOrig = unAddress;

    AccessType armAccessType = AccessType::NonSequential;

    if (ubyRegisterList != 0) {
	if (bLoad) {
	    if (ubyRegisterList & (1 << usnRegisterB))
		bWriteBack = false;

	    for (int i = 0; i < 8; i++) {
		if (ubyRegisterList & (1 << i)) {
		    m_aRegisters[i] = m_Mmu.ReadWord(unAddress, armAccessType);
		    armAccessType = AccessType::Sequential;
		    unAddress += 4;
		}
	    }

	    // Idle
	} else {
	    auto lamCountTrailingZeros = [&]() {
		uint32_t unRegListCpy = ubyRegisterList;
		uint32_t unZeros = 0;
		while (unRegListCpy) {
		    if ((unRegListCpy & 0x1) == 0)
			unZeros++;
		    else
			break;
		    unRegListCpy >>= 1;
		}
		return unZeros;
	    };

	    auto lamBitCount = [&]() {
		uint32_t unRegListCpy = ubyRegisterList;
		uint32_t unBitCount = 0;
		while (unRegListCpy) {
		    if (unRegListCpy & 0x1)
			unBitCount++;
		    unRegListCpy >>= 1;
		}
		return unBitCount;
	    };

	    uint32_t unRegisterListTrailingZero = lamCountTrailingZeros();

	    for (int i = 0; i < 8; i++) {
		if (ubyRegisterList & (1 << i)) {
		    if (i == usnRegisterB) {
			if (usnRegisterB == unRegisterListTrailingZero)
			    m_Mmu.WriteWord(unAddress, unBaseOrig, AccessType::NonSequential);
			else
			    m_Mmu.WriteWord(unAddress, unBaseOrig + 4 * lamBitCount(), AccessType::NonSequential);
		    } else {
			m_Mmu.WriteWord(m_aRegisters[i], unAddress, armAccessType);
		    }
		    armAccessType = AccessType::Sequential;
		    unAddress += 4;
		}
	    }
	}
    } else {
	if (bLoad) {
	    m_PC = m_Mmu.ReadWord(unAddress, AccessType::NonSequential);
	    FlushPipelineTHUMB();
	}
	else {
	    m_Mmu.WriteWord(unAddress, m_PC, AccessType::NonSequential);
	}
	unAddress += 0x40;
    }

    if (bWriteBack)
	m_aRegisters[usnRegisterB] = unAddress;
}