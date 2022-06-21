/*+==============================================================================
  File:      ARM7TDMI.cpp

  Summary:   Defines the utility methods of the ARM7TDMI class

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::TestCondtion, ARM7TDMI::FlushPipelineARM,
	     ARM7TDMI::FlushPipelineTHUMB, ARM7TDMI::SwitchMode,
	     ARM7TDMI::Clock

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

#include "ARM7TDMI.h"

ARM_FunctionPointer ARM7TDMI::m_aarmInstructionTable[0x1000];
THUMB_FunctionPointer ARM7TDMI::m_atmbInstructionTable[0x400];

bool ARM7TDMI::TestCondition(ConditionField armCondField) {
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
    m_PC &= ~0b11;
    m_aInstructionPipe[0] = m_Mmu.ReadWord(m_PC, AccessType::NonSequential);
    m_aInstructionPipe[1] = m_Mmu.ReadWord(m_PC + 4, AccessType::Sequential);
    m_CpuAccessType = AccessType::Sequential;
    m_PC += 4;
}

void ARM7TDMI::FlushPipelineTHUMB() {
    // Align address
    m_PC &= ~0b1;
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

ARM7TDMI::ARM7TDMI() {
    std::fill(std::begin(m_aRegisters), std::end(m_aRegisters), 0);
    m_Mmu.InitTest();
    m_SP = 0x03007F00;
    m_LR = 0x00000000;
    m_PC = 0x08000000;
    m_SPSR.Register = 0x00000000;
    m_CPSR.Register = 0x0000001F;
    FlushPipelineARM();

    m_aRegisterBanks[uint32_t(CPU_BankMode::USRnSYS)][0] = 0x03007F00;
    m_aRegisterBanks[uint32_t(CPU_BankMode::USRnSYS)][1] = 0x000000C0;
    m_aRegisterBanks[uint32_t(CPU_BankMode::IRQ)][0] = 0x03007FA0;
    m_aRegisterBanks[uint32_t(CPU_BankMode::SVC)][0] = 0x03007FE0;

    m_CpuExecutionState = 0;

    m_PC += 4;
}

void ARM7TDMI::Clock() {
    if (m_CpuExecutionState & static_cast<uint8_t>(ExecutionState::THUMB)) {
	unInstruction = m_aInstructionPipe[0];
	m_aInstructionPipe[0] = m_aInstructionPipe[1];
	m_aInstructionPipe[1] = m_Mmu.ReadHalfWord(m_PC, m_CpuAccessType);
	// Execute instruction
	std::invoke(m_atmbInstructionTable[HashThumbOpcode(unInstruction)], this, unInstruction);
    } else {
	unInstruction = m_aInstructionPipe[0];
	m_aInstructionPipe[0] = m_aInstructionPipe[1];
	m_aInstructionPipe[1] = m_Mmu.ReadWord(m_PC, m_CpuAccessType);
	// Check condition then execute
	if (TestCondition(static_cast<ConditionField>(unInstruction >> 28)))
	    std::invoke(m_aarmInstructionTable[HashArmOpcode(unInstruction)], this, unInstruction);
    }
    m_PC += m_CPSR.T ? 2 : 4;
}