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