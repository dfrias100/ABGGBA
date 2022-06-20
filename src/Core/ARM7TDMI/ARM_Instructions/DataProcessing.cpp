#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::DataProcessing(uint32_t unInstruction) {
    bool bOperand2IsImmediate = (unInstruction & 0x02000000) >> 25;
    bool bSetConditionCodes = (unInstruction & 0x00100000) >> 20;
    uint8_t byOpCode = (unInstruction & 0x01E00000) >> 21;

    uint16_t usnRegOperand1 = (unInstruction & 0x000F0000) >> 16;
    uint16_t usnRegDest     = (unInstruction & 0x0000F000) >> 12;

    uint16_t usnOperand2 = (unInstruction & 0x00000FFF);

    uint32_t& unrDestinationRegister = m_aRegisters[usnRegDest];
    uint32_t unOperand1 = m_aRegisters[usnRegOperand1];
    uint32_t unOperand2;

    bool bSetAndNotPC = bSetConditionCodes && (usnRegDest != 15);
    bool bIsLogicalOpCode = static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::AND ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::EOR ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::TST ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::TEQ ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::ORR ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::MOV ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::BIC ||
			    static_cast<DataProcessingOpCode>(byOpCode) == DataProcessingOpCode::MVN;
			    
    if (bOperand2IsImmediate) {
	uint32_t unRotateAmount = (usnOperand2 & 0xF00) >> 8;
	unOperand2 = usnOperand2 & 0xFF;
	unOperand2 = ROR(unOperand2, unRotateAmount * 2, bSetAndNotPC && bIsLogicalOpCode, false);
    } else {
	// Operand 2 is a register, the upper 8 bits
	// of the 12-bit operand 2 field is a shift
	uint16_t usnShiftControl = usnOperand2 >> 4;
	uint16_t usnRegisterSelect = usnShiftControl >> 4;
	uint32_t unShiftAmount;
	bool bIdleCycle = false;

	unOperand2 = m_aRegisters[usnOperand2 & 0xF];

	if (usnShiftControl & 0x1) {
	    unShiftAmount = m_aRegisters[usnRegisterSelect] & 0xFF;

	    // R15, the PC, is already 8 bytes ahead, if 
	    // a register is used to specify the shift,
	    // it has to be 12 bytes ahead, so we add 4
	    // more bytes
	    if (usnRegOperand1 == 15)	   unOperand1 += 4;
	    if ((usnOperand2 & 0xF) == 15) unOperand2 += 4;

	    bIdleCycle = true;
	} else {
	    unShiftAmount = usnShiftControl >> 3;
	}

	usnShiftControl = (usnShiftControl >> 1) & 3;

	switch (usnShiftControl) {
	    case 0x00: unOperand2 = LSL(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode); break;
	    case 0x01: unOperand2 = LSR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
	    case 0x02: unOperand2 = ASR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
	    case 0x03: unOperand2 = ROR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
	}

	// Idle here when needed
	//if (bIdleCycle)
	    // TODO: Handle idling
    }

    bool bNotWriteBack = false;

    switch (static_cast<DataProcessingOpCode>(byOpCode)) {
    case DataProcessingOpCode::AND: unrDestinationRegister = MSC(unOperand1 & unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::EOR: unrDestinationRegister = MSC(unOperand1 ^ unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::SUB: unrDestinationRegister = SUB(unOperand1, unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::RSB: unrDestinationRegister = SUB(unOperand2, unOperand1, bSetAndNotPC); break;
    case DataProcessingOpCode::ADD: unrDestinationRegister = ADD(unOperand1, unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::ADC: unrDestinationRegister = ADC(unOperand1, unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::SBC: unrDestinationRegister = SBC(unOperand1, unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::RSC: unrDestinationRegister = SBC(unOperand2, unOperand1, bSetAndNotPC); break;
    case DataProcessingOpCode::TST: bNotWriteBack = true;    MSC(unOperand1 & unOperand2, true); break;
    case DataProcessingOpCode::TEQ: bNotWriteBack = true;    MSC(unOperand1 ^ unOperand2, true); break;
    case DataProcessingOpCode::CMP: bNotWriteBack = true;    SUB(unOperand1, unOperand2, true); break;
    case DataProcessingOpCode::CMN: bNotWriteBack = true;    ADD(unOperand1, unOperand2, true); break;
    case DataProcessingOpCode::ORR: unrDestinationRegister = MSC(unOperand1 | unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::MOV: unrDestinationRegister = MSC(unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::BIC: unrDestinationRegister = MSC(unOperand1 & ~unOperand2, bSetAndNotPC); break;
    case DataProcessingOpCode::MVN: unrDestinationRegister = MSC(~unOperand2, bSetAndNotPC); break;
    }

    if (usnRegDest == 15) {
	if (bSetConditionCodes) {
	    // Restore CPSR and switch modes
	    PSR armSpsr = m_SPSR;
	    SwitchMode(static_cast<CPU_Mode>(armSpsr.Mode));
	    m_CPSR = armSpsr;
	}

	if (!bNotWriteBack) {
	    if (m_CPSR.T) {
		// Flush pipelines and change state to thumb
		FlushPipelineTHUMB();
		m_CpuExecutionState |= static_cast<uint8_t>(ExecutionState::THUMB);
	    } else {
		// Flush pipelines for ARM sized instructions
		m_CpuExecutionState &= ~static_cast<uint8_t>(ExecutionState::THUMB);
		FlushPipelineARM();
	    }
	}
    }
}

void ARM7TDMI::PSR_Transfer(uint32_t unInstruction) {
    uint32_t unMrsOrMsr = (unInstruction & (0x3FF << 12)) >> 12;

    bool bIsMrs = (unMrsOrMsr & 0b11'1111'0000) == 0b00'1111'0000;
    bool bCpsrOrSpsr = (unInstruction & (1 << 22)) != 0;

    if (bIsMrs) {
	// MRS instruction
	uint16_t usnRegisterDestination = unMrsOrMsr & 0xF;
	m_aRegisters[usnRegisterDestination] = bCpsrOrSpsr ? m_SPSR.Register : m_CPSR.Register;
    } else {
	// MSR instruction
	bool bMsrWithRegister = (unMrsOrMsr & 0b10000) ? true : false;
	uint32_t& unPsrDest = bCpsrOrSpsr ? m_SPSR.Register : m_CPSR.Register;

	if (bMsrWithRegister) {
	    uint32_t unValToTransfer = m_aRegisters[unInstruction & 0xF];

	    unValToTransfer &= 0xF00000FF;
	    unValToTransfer |= m_CPSR.Register & 0x0FFFFF00;

	    if (!bCpsrOrSpsr) {
		if (static_cast<CPU_Mode>(m_CPSR.Mode) == CPU_Mode::USR) {
		    unValToTransfer &= ~0xFF;
		    unValToTransfer |= m_CPSR.Register & 0xFF;
		} else {
		    SwitchMode(static_cast<CPU_Mode>(unValToTransfer & 0x1F));
		    if (unValToTransfer & 0x20) {
			// The address this instruction occurred at PC - 8, the next
			// thumb instruction is then at PC - 4, if the CPSR changes then
			// we need to refill the pipeline and set the PC to the 
			// next instruction
			m_PC -= 4;
			FlushPipelineTHUMB();
			m_CpuExecutionState |= static_cast<uint8_t>(ExecutionState::THUMB);
		    }
		}
	    }
	    
	    unPsrDest = unValToTransfer;
	} else {
	    //  MSR flag bits transfer
	    bool bFlagsAreImmediate = (unInstruction & (1 << 25)) != 0;
	    uint32_t unFlagsValue;

	    if (bFlagsAreImmediate) {
		uint32_t unImmValue = unInstruction & 0xFF;
		uint16_t byRotate = (unInstruction & 0xF00) >> 8;
		unFlagsValue = ROR(unImmValue, byRotate * 2, false, false);
	    } else {
		unFlagsValue = m_aRegisters[unInstruction & 0xF];
	    }

	    unPsrDest &= ~0xF0000000;
	    unFlagsValue &= 0xF0000000;

	    unPsrDest |= unFlagsValue;
	}
    }
}

#include "../Arithmetic.inl"
#include "../Shifts.inl"