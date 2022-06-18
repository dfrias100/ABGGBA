#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::AddOffToSP(uint16_t usnInstruction) {
    bool bSign = (usnInstruction & 0x80) != 0;
    uint32_t unOffset = usnInstruction & 0x7F;

    unOffset <<= 2;
    
    if (bSign)
	m_SP -= unOffset;
    else
	m_SP += unOffset;
}

void ARM7TDMI::PushOrPopRegs(uint16_t usnInstruction) {
    bool bLoad = (usnInstruction & 0x800) >> 11;
    bool bAlterPcLr = (usnInstruction & 0x100) >> 8;

    uint16_t ubyRegisterList = usnInstruction & 0xFF;

    AccessType armAccessType = AccessType::NonSequential;

    if (bLoad) {
	for (int i = 0; i < 8; i++) {
	    if (ubyRegisterList & (i << 1)) {
		m_aRegisters[i] = m_Mmu.ReadWord(m_SP, armAccessType);
		armAccessType = AccessType::Sequential;
		m_SP += 4;
	    }
	}

	// Idle

	if (bAlterPcLr) {
	    m_PC = m_Mmu.ReadWord(m_SP, armAccessType);
	    m_SP += 4;
	    FlushPipelineTHUMB();
	}
    } else {
	if (bAlterPcLr) {
	    m_SP -= 4;
	    m_Mmu.WriteWord(m_LR, m_SP, armAccessType);
	}

	for (int i = 7; i >= 0; i--) {
	    if (ubyRegisterList & (i << 1)) {
		m_SP -= 4;
		m_Mmu.WriteWord(m_aRegisters[i], m_SP, armAccessType);
		armAccessType = AccessType::Sequential;
	    }
	}
    }
}