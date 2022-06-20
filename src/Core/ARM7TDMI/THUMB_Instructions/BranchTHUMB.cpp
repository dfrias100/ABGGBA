#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::UnconditionalBranch(uint16_t usnInstruction) {
    uint32_t unOffset = usnInstruction & 0x00'00'07'FF;

    if (unOffset & 0x00'00'04'00)
	unOffset |= 0xFF'FF'F8'00;

    unOffset <<= 1;

    m_PC += unOffset;
    FlushPipelineTHUMB();
}

void ARM7TDMI::ConditionalBranch(uint16_t usnInstruction) {
    if (TestCondition(static_cast<ConditionField>((usnInstruction & 0xF00) >> 8))) {
	uint32_t unOffset = usnInstruction & 0x00'00'00'FF;

	if (unOffset & 0x00'00'00'80)
	    unOffset |= 0xFF'FF'FF'00;

	m_PC += unOffset;
	FlushPipelineTHUMB();
    }
}

void ARM7TDMI::LongBranchWithLink(uint16_t usnInstruction) {
    uint32_t unOffset = usnInstruction & 0x00'00'07'FF;
    bool bOffsetLow = (usnInstruction & (1 << 11)) != 0;

    if (bOffsetLow) {
	uint32_t unNextInstruction = m_PC - 2;
	m_PC = m_LR + (unOffset << 1);
	m_LR = unNextInstruction | 1;
	FlushPipelineTHUMB();
    } else {
	if (unOffset & 0x00'00'04'00)
	    unOffset |= 0xFF'FF'F8'00;
	m_LR = m_PC + (unOffset << 12);
    }
}