#include "../ARM7TDMI.h"

void ARM7TDMI::SingleDataSwap(uint32_t unInstruction) {
    bool bByteQuantity = (unInstruction & (1 << 22));

    uint32_t unRegisterN = (unInstruction & (0xF << 16));
    uint32_t unRegisterD = (unInstruction & (0xF << 12));
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
}

void ARM7TDMI::HalfwordDataTransfer(uint32_t unInstruction) {
    bool bPreIndexing = (unInstruction & (1 << 24)) != 0;
    bool bUpOffset    = (unInstruction & (1 << 23)) != 0;
    bool bImmOffset   = (unInstruction & (1 << 22)) != 0;
    bool bWriteBack   = (unInstruction & (1 << 21)) != 0;
    bool bLoad        = (unInstruction & (1 << 20)) != 0;

    uint32_t unDataType = (unInstruction & (0b0110 << 4)) >> 4;

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

	bool bIsLDRH = unDataType == 0x01;
	bool bIsLDRSH = unDataType == 0x11;

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

	if (unDataType & 0x80)
	    lamSignExtend(unData, unDataType & 0x1);

	// Idle

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

}

void ARM7TDMI::BlockDataTransfer(uint32_t unInstruction) {

}