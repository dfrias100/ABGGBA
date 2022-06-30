/*+==============================================================================
  File:      ARM7TDMI.h

  Summary:   Defines the ARM7TDMI class and declares its functions

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::HashArmOpcode, ARM7TDMI::HashThumbOpcode

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

#ifndef ARM7TDMI_H
#define ARM7TDMI_H

#include <cstdint>
#include <algorithm>
#include <functional>

#include "../Memory/Memory.h"
#include "../Memory/AccessType.h"
#include "Instruction.h"

// Forward Declarations
enum class ConditionField;
enum class ARM_Operation;
enum class THUMB_Operation;
struct ARM_Instruction;
struct THUMB_Instruction;

enum class CPU_BankMode {
    USRnSYS = 0,
    IRQ = 1,
    SVC = 2,
    ABT = 3,
    UND = 4,
    FIQ = 5
};

enum class CPU_Mode {
    USR = 0b1'0000, // 0x10
    FIQ = 0b1'0001, // 0x11
    IRQ = 0b1'0010, // 0x12
    SVC = 0b1'0011, // 0x13
    ABT = 0b1'0111, // 0x17
    UND = 0b1'1011, // 0x1B
    SYS = 0b1'1111  // 0x1F
};

enum class ExecutionState {
    THUMB = (1 << 1),
    Halted = (1 << 2),
    IRQ = (1 << 3),
    DMA = (1 << 4)
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    ARM7TDMI

  Summary:  Implementation of the ARM7TDMI CPU in the GBA; this is
	    an interpreter of its instructions

  Methods:  Please refer to the other .cpp files in this directory
	    and their subdirectories
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class ARM7TDMI {
public:
    ARM7TDMI();
    void Clock();
    static void GenerateInstructionTables();
    Memory m_Mmu;
private:
    uint32_t unInstruction;

    // Anonymous union-structs are very helpful
    // here to access data like this
    union {
	struct {
	    uint32_t m_aGeneralPurposeRegisters[13];
	    uint32_t m_SP;
	    uint32_t m_LR;
	    uint32_t m_PC;
	};
	uint32_t m_aRegisters[16];
    };

    union PSR {
	struct {
	    // We could store some of these as 8-bit
	    // ints but we're specifying the number
	    // of bits we're using anyway
	    uint32_t Mode : 5;
	    uint32_t T : 1;
	    uint32_t F : 1;
	    uint32_t I : 1;
	    uint32_t Reserved : 20;
	    uint32_t V : 1;
	    uint32_t C : 1;
	    uint32_t Z : 1;
	    uint32_t N : 1;
	};
	uint32_t Register;
    };

    PSR m_CPSR;
    PSR m_SPSR;

    /*-------------------------------------------------------------
       For 6 out of the 7 modes, there are 3 banks that store the
       SP, LR, and CPSR of the mode

       2 of the modes, USR and SYS, share the same registers

       Major Indices 0-5: See the enum class CPU_BankMode at 
			  the top of this file
       Minor Index 2: SP_mode
       Minor Index 1: LR_mode
       Minor Index 0: SPSR_mode
    -------------------------------------------------------------*/
    uint32_t m_aRegisterBanks[6][3];

    /*----------------------------------------------------------------
	If we're in FIQ mode, we switched out from a 3-banked mode
	into the 8-banked FIQ mode, so we need to keep more registers

	Major Index 0: Data from old mode
	Major Index 1: FIQ registers

	Minor Index 4 - 0: R8_mode - R12_mode
    ----------------------------------------------------------------*/
    uint32_t m_aFiqRegisterBanks[2][5];

    /*----------------------------------------------------------------
	This pipeline contains the next two instructions in memory;
	PC will always contain the current instruction, when we step
	it will look like this:
	    Op = m_aInstructionPipe[0];
	    m_aInstructionPipe[0] = m_aInstructionPipe[1];
	    m_aInstructionPipe[1] = ReadMem(PC);
	"ReadMem" will, of course, differ if we are in thumb state
	or not.
    ----------------------------------------------------------------*/
    uint32_t m_aInstructionPipe[2];
    AccessType m_CpuAccessType;

    uint8_t m_CpuExecutionState;

    // ARM instructions
    void DataProcessing(uint32_t unInstruction);
    void PSR_Transfer(uint32_t unInstruction);
    void Multiply(uint32_t unInstruction);
    void MultiplyLong(uint32_t unInstruction);
    void SingleDataSwap(uint32_t unInstruction);
    void BranchAndExchange(uint32_t unInstruction);
    void HalfwordDataTransfer(uint32_t unInstruction);
    void SingleDataTransfer(uint32_t unInstruction);
    void BlockDataTransfer(uint32_t unInstruction);
    void Branch(uint32_t unInstruction);
    void SoftwareInterruptARM(uint32_t unInstruction);
    void UnimplementedInstructionARM(uint32_t unInstruction);
    

    // THUMB instructions
    void MoveShiftedRegister(uint16_t usnInstruction);
    void AddSubtract(uint16_t usnInstruction);
    void MoveCompareAddSubtractImm(uint16_t usnInstruction);
    void ALU(uint16_t usnInstruction);
    void HiRegisterOpsOrBranchExchange(uint16_t usnInstruction);
    void PC_RelLoad(uint16_t usnInstruction);
    void LoadOrStoreWithRegOff(uint16_t usnInstruction);
    void LoadOrStoreSignExtendedByteOrHalfWord(uint16_t usnInstruction);
    void LoadOrStoreWithImmOff(uint16_t usnInstruction);
    void LoadOrStoreHalfWord(uint16_t usnInstruction);
    void SP_RelLoadOrStore(uint16_t usnInstruction);
    void LoadAddress(uint16_t usnInstruction);
    void AddOffToSP(uint16_t usnInstruction);
    void PushOrPopRegs(uint16_t usnInstruction);
    void MultipleLoadOrStore(uint16_t usnInstruction);
    void UnconditionalBranch(uint16_t unInstruction);
    void SoftwareInterruptTHUMB(uint16_t usnInstruction);
    void ConditionalBranch(uint16_t usnInstruction);
    void LongBranchWithLink(uint16_t usnInstruction);
    void UnimplementedInstructionTHUMB(uint16_t usnInstruction);

    // Instruction Decoding
    static ARM_FunctionPointer m_aarmInstructionTable[0x1000];
    static THUMB_FunctionPointer m_atmbInstructionTable[0x400];

    static constexpr inline uint16_t HashArmOpcode(uint32_t unArmOpcode);
    static constexpr inline uint16_t HashThumbOpcode(uint16_t unThumbOpcode);

    template <uint32_t Instruction>
    static constexpr ARM_Instruction DecodeARM_Instruction();

    template <uint16_t Instruction>
    static constexpr THUMB_Instruction DecodeTHUMB_Instruction();

    static constexpr void GenerateARM_Tables(ARM_FunctionPointer aarmInstructionTable[0x1000]);
    static constexpr void GenerateTHUMB_Tables(THUMB_FunctionPointer atmbInstructionTable[0x400]);

    bool TestCondition(ConditionField armCondField);

    // Shifts
    inline uint32_t LSL(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags);
    inline uint32_t LSR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate);
    inline uint32_t ASR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate);
    inline uint32_t ROR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate);

    // ALU Operations
    inline uint32_t SUB(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags);
    inline uint32_t ADD(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags);
    inline uint32_t ADC(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags);
    inline uint32_t SBC(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags);
    inline uint32_t MSC(uint32_t unExpression, bool bAffectFlags);

    // Utility Functions
    void FlushPipelineARM();
    void FlushPipelineTHUMB();
    void SwitchMode(CPU_Mode armMode);
};						    

constexpr inline uint16_t ARM7TDMI::HashArmOpcode(uint32_t unArmOpcode) {
    return ((unArmOpcode & 0x0FF00000) >> 16) | ((unArmOpcode & 0xF0) >> 4);
}

constexpr inline uint16_t ARM7TDMI::HashThumbOpcode(uint16_t unThumbOpcode) {
    return unThumbOpcode >> 6;
}

#include "Arithmetic.inl"
#include "Shifts.inl"

#endif