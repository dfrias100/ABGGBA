#include "../ARM7TDMI.h"
#include "../Instruction.h"
#include <iostream>
#include <iomanip>

void ARM7TDMI::MoveShiftedRegister(uint16_t usnInstruction) {
    uint16_t usnOpcode    = (usnInstruction & 0b0001'1000'0000'0000) >> 11;
    uint16_t usnOffset    = (usnInstruction & 0b0000'0111'1100'0000) >>  6;
    uint16_t usnRegisterS = (usnInstruction & 0b0000'0000'0011'1000) >>  3;
    uint16_t usnRegisterD = (usnInstruction & 0b0000'0000'0000'0111);

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];
    uint32_t  unSourceRegister = m_aRegisters[usnRegisterS];

    switch (usnOpcode) {
    case 0: unDestinationRegister = MSC(LSL(unSourceRegister, usnOffset, true), true); break;
    case 1: unDestinationRegister = MSC(LSR(unSourceRegister, usnOffset, true, true), true); break;
    case 2: unDestinationRegister = MSC(ASR(unSourceRegister, usnOffset, true, true), true); break;
    }
}

void ARM7TDMI::HiRegisterOpsOrBranchExchange(uint16_t usnInstruction) {
    uint16_t usnOpcode = (usnInstruction & 0b11'0000'0000) >> 8;
    uint32_t unRegisterS = (usnInstruction & 0b111'000) >> 3;
    uint32_t unRegisterD = usnInstruction & 0b000'111;
    bool bHiRegister1 = (usnInstruction & (1 << 7)) != 0;
    bool bHiRegister2 = (usnInstruction & (1 << 6)) != 0;

    if (bHiRegister1)
	unRegisterD |= (1 << 3);

    if (bHiRegister2)
	unRegisterS |= (1 << 3);

    uint32_t& unRegisterDestination = m_aRegisters[unRegisterD];
    uint32_t  unRegisterSource = m_aRegisters[unRegisterS];

    switch (usnOpcode)
    {
    case 0: // ADD
	unRegisterDestination += unRegisterSource;
	if (unRegisterD == 15)
	    FlushPipelineTHUMB();
	break;
    case 1: // CMP
	SUB(unRegisterDestination, unRegisterSource, true);
	break;
    case 2: // MOV
	unRegisterDestination = unRegisterSource;
	if (unRegisterD == 15)
	    FlushPipelineTHUMB();
	break;
    case 3: // Branch and Exchange
	m_PC = unRegisterSource;
	m_CPSR.T = unRegisterSource & 0x1;
	if (m_CPSR.T) {
	    m_CpuExecutionState |= static_cast<uint8_t>(ExecutionState::THUMB);
	    FlushPipelineTHUMB();
	} else {
	    m_CpuExecutionState &= ~static_cast<uint8_t>(ExecutionState::THUMB);
	    FlushPipelineARM();
	}
	break;
    }
}

void ARM7TDMI::SoftwareInterruptTHUMB(uint16_t usnInstruction) {

}

void ARM7TDMI::UnimplementedInstructionTHUMB(uint16_t usnInstruction) {
    std::cerr << "WARN: Unknown or undefined instruction " << std::hex << std::setw(8) << unInstruction <<
	" executed at PC = 0x" << std::hex << std::setw(8) << m_PC - 4 << std::endl;
}