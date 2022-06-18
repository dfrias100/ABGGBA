#include "ARM7TDMI.h"

inline bool ARM7TDMI::TestCondition(ConditionField armCondField) {
    switch (armCondField) {
    case ConditionField::EQ:

	return m_CPSR.Z == 1;

    case ConditionField::NE:

	return m_CPSR.Z == 0;

    case ConditionField::CS:

	return m_CPSR.C == 1;

    case ConditionField::CC:

	return m_CPSR.C == 0;

    case ConditionField::MI:

	return m_CPSR.N == 1;

    case ConditionField::PL:

	return m_CPSR.N == 0;

    case ConditionField::VS:

	return m_CPSR.V == 1;

    case ConditionField::VC:

	return m_CPSR.V == 0;

    case ConditionField::HI:

	return (m_CPSR.C == 1) && (m_CPSR.Z == 0);

    case ConditionField::LS:

	return (m_CPSR.C == 0) || (m_CPSR.Z == 1);

    case ConditionField::GE:

	return (m_CPSR.N == m_CPSR.V);

    case ConditionField::LT:

	return (m_CPSR.N != m_CPSR.V);

    case ConditionField::GT:

	return (m_CPSR.Z == 0) && (m_CPSR.N == m_CPSR.V);

    case ConditionField::LE:

	return (m_CPSR.Z == 1) || (m_CPSR.N != m_CPSR.V);

    case ConditionField::AL:
    default:
	return true;
    };
}

void ARM7TDMI::FlushPipelineARM() {
    // Align address
    m_PC &= 0b11;
    m_aInstructionPipe[0] = m_Mmu.ReadWord(m_PC, AccessType::NonSequential);
    m_aInstructionPipe[1] = m_Mmu.ReadWord(m_PC + 4, AccessType::Sequential);
    m_CpuAccessType = AccessType::Sequential;
    m_PC += 4;
}

void ARM7TDMI::FlushPipelineTHUMB() {
    // Align address
    m_PC &= 0b1;
    m_aInstructionPipe[0] = m_Mmu.ReadWord(m_PC, AccessType::NonSequential);
    m_aInstructionPipe[1] = m_Mmu.ReadWord(m_PC + 2, AccessType::Sequential);
    m_CpuAccessType = AccessType::Sequential;
    m_PC += 2;
}

void ARM7TDMI::SwitchMode(CPU_Mode armMode) {
    auto lamGetBank = [&](CPU_Mode armMode) {
	switch (armMode) {
	case CPU_Mode::USR:
	case CPU_Mode::SYS:
	    return CPU_BankMode::USRnSYS;
	case CPU_Mode::IRQ:
	    return CPU_BankMode::IRQ;
	case CPU_Mode::FIQ:
	    return CPU_BankMode::FIQ;
	case CPU_Mode::SVC:
	    return CPU_BankMode::SVC;
	case CPU_Mode::UND:
	    return CPU_BankMode::UND;
	case CPU_Mode::ABT:
	    return CPU_BankMode::ABT;
	}
    };

    CPU_BankMode armNewMode = lamGetBank(armMode);
    CPU_BankMode armOldMode = lamGetBank(static_cast<CPU_Mode>(m_CPSR.Mode));

    if (armNewMode != CPU_BankMode::FIQ && armOldMode != CPU_BankMode::FIQ) {
	if (armNewMode != armOldMode) {
	    // Save the registers
	    m_aRegisterBanks[uint32_t(armOldMode)][2] = m_SP;
	    m_aRegisterBanks[uint32_t(armOldMode)][1] = m_LR;
	    m_aRegisterBanks[uint32_t(armOldMode)][0] = m_SPSR.Register;

	    m_SP = m_aRegisterBanks[uint32_t(armNewMode)][2];
	    m_LR = m_aRegisterBanks[uint32_t(armNewMode)][1];
	    m_SPSR.Register = m_aRegisterBanks[uint32_t(armNewMode)][0];
	}
    } else {
	uint32_t unFiqIdxOld = armOldMode == CPU_BankMode::FIQ;
	uint32_t unFiqIdxNew = armNewMode == CPU_BankMode::FIQ;

	m_aFiqRegisterBanks[unFiqIdxOld][7] = m_aRegisters[8];
	m_aFiqRegisterBanks[unFiqIdxOld][6] = m_aRegisters[9];
	m_aFiqRegisterBanks[unFiqIdxOld][5] = m_aRegisters[10];
	m_aFiqRegisterBanks[unFiqIdxOld][4] = m_aRegisters[11];
	m_aFiqRegisterBanks[unFiqIdxOld][3] = m_aRegisters[12];
	m_aFiqRegisterBanks[unFiqIdxOld][2] = m_aRegisters[13];
	m_aFiqRegisterBanks[unFiqIdxOld][1] = m_aRegisters[14];
	m_aFiqRegisterBanks[unFiqIdxOld][0] = m_SPSR.Register;


	m_aRegisters[8]  = m_aFiqRegisterBanks[unFiqIdxNew][7];
	m_aRegisters[9]  = m_aFiqRegisterBanks[unFiqIdxNew][6];
	m_aRegisters[10] = m_aFiqRegisterBanks[unFiqIdxNew][5];
	m_aRegisters[11] = m_aFiqRegisterBanks[unFiqIdxNew][4];
	m_aRegisters[12] = m_aFiqRegisterBanks[unFiqIdxNew][3];
	m_aRegisters[13] = m_aFiqRegisterBanks[unFiqIdxNew][2];
	m_aRegisters[14] = m_aFiqRegisterBanks[unFiqIdxNew][1];
	m_SPSR.Register  = m_aFiqRegisterBanks[unFiqIdxNew][0];
    }

    m_CPSR.Mode = uint32_t(armNewMode);
}

void ARM7TDMI::Clock() {
    if (m_CpuExecutionState & static_cast<uint8_t>(ExecutionState::THUMB)) {
	unInstruction = m_aInstructionPipe[0];
	m_aInstructionPipe[0] = m_aInstructionPipe[1];
	m_aInstructionPipe[1] = m_Mmu.ReadHalfWord(m_PC, m_CpuAccessType);
	// Execute instruction
    } else {
	unInstruction = m_aInstructionPipe[0];
	m_aInstructionPipe[0] = m_aInstructionPipe[1];
	m_aInstructionPipe[1] = m_Mmu.ReadWord(m_PC, m_CpuAccessType);
	// Check condition then execute
    }
    m_PC += m_CPSR.T ? 2 : 4;
}