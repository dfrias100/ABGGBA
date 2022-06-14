#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::DataProcessing(uint32_t unInstruction) {
    bool bOperand2IsImmediate = (unInstruction & 0x02000000) >> 25;
    bool bSetConditionCodes = (unInstruction & 0x00100000) >> 20;
    uint8_t byOpCode = (unInstruction & 0x01E00000) >> 21;

    uint16_t usnRegOperand1 = (unInstruction & 0x000F0000) >> 16;
    uint16_t usnRegDest = (unInstruction & 0x0000F000) >> 12;

    uint16_t usnOperand2 = (unInstruction & 0x00000FFF);

    uint32_t& unrDestinationRegister = m_aRegisters[usnRegDest];
    uint32_t unOperand1 = m_aRegisters[usnRegOperand1];
    uint32_t unOperand2;

    bool bSetAndNotPC = (unInstruction & (1 << 20)) && (usnRegDest != 15);
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
	    unShiftAmount = m_aRegisters[usnRegisterSelect];

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
	    case 00: LSL(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode); break;
	    case 01: LSR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
	    case 02: ASR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
	    case 03: ROR(unOperand2, unShiftAmount, bSetAndNotPC && bIsLogicalOpCode, !bIdleCycle); break;
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
	//if ((unInstruction & (1 << 20)))
	    // Restore CPSR and switch modes

	if (!bNotWriteBack) {
	    if (m_CPSR.T) {
		// Flush pipelines and change state to thumb
	    } else {
		// Flush pipelines for ARM sized instructions
	    }
	}
    }
}

void ARM7TDMI::PSR_Transfer(uint32_t unInstruction) {
}

#include "../Arithmetic.inl"
#include "../Shifts.inl"