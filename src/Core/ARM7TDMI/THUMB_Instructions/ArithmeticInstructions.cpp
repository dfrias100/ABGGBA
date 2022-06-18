#include "../ARM7TDMI.h"
#include "../Instruction.h"

void ARM7TDMI::AddSubtract(uint16_t usnInstruction) {
    bool bIsImmediate = (usnInstruction & (1 << 10)) != 0;
    bool bSubtract    = (usnInstruction & (1 <<  9)) != 0;

    uint16_t usnRnOrOffset = (usnInstruction & 0b111'000'000) >> 6;
    uint16_t usnRegisterS  = (usnInstruction & 0b000'111'000) >> 3;
    uint16_t usnRegisterD  = (usnInstruction & 0b000'000'111);

    uint32_t& unDestinationRegister = m_aRegisters[usnRegisterD];
    uint32_t  unSourceRegister      = m_aRegisters[usnRegisterS];

    uint32_t unOperand;

    if (bIsImmediate) {
	unOperand = usnRnOrOffset;
    } else {
	unOperand = m_aRegisters[usnRnOrOffset];
    }

    if (bSubtract) {
	unDestinationRegister = SUB(unSourceRegister, unOperand, true);
    } else {
	unDestinationRegister = ADD(unSourceRegister, unOperand, true);
    }
}

void ARM7TDMI::MoveCompareAddSubtractImm(uint16_t usnInstruction) {
    uint16_t usnOpcode = (usnInstruction & (0b11 << 11)) >> 11;
    uint16_t usnRegisterD = (usnInstruction & 0b111'0000'0000) >> 8;
    uint8_t ubyOffset = usnInstruction & 0xFF;

    uint32_t& unSourceDestRegister = m_aRegisters[usnRegisterD];

    switch (usnOpcode) {
    case 0: unSourceDestRegister = MSC(ubyOffset, true); break;
    case 1:			   SUB(unSourceDestRegister, ubyOffset, true); break;
    case 2: unSourceDestRegister = ADD(unSourceDestRegister, ubyOffset, true); break;
    case 3: unSourceDestRegister = SUB(unSourceDestRegister, ubyOffset, true); break;
    }
}

enum class ALU_OpCode{
    AND = 0b0000,
    EOR = 0b0001,
    LSL = 0b0010,
    LSR = 0b0011,
    ASR = 0b0100,
    ADC = 0b0101,
    SBC = 0b0110,
    ROR = 0b0111,
    TST = 0b1000,
    NEG = 0b1001,
    CMP = 0b1010,
    CMN = 0b1011,
    ORR = 0b1100,
    MUL = 0b1101,
    BIC = 0b1110,
    MVN = 0b1111
};

void ARM7TDMI::ALU(uint16_t usnInstruction) {
    uint16_t usnOpcode = (usnInstruction & 0b1111'000'000) >> 6;
    uint8_t ubyRegisterS = (usnInstruction & 0b111'000) >> 3;
    uint8_t ubyRegisterD = (usnInstruction & 0b000'111);

    uint32_t& unDestinationRegister = m_aRegisters[ubyRegisterD];
    uint32_t  unSourceRegister      = m_aRegisters[ubyRegisterS];

    int32_t nM = 0;

    auto lamReducedMUL = [&]() {
	uint32_t ulResult = unDestinationRegister * unSourceRegister;

	// Set condition codes
	MSC(ulResult, true);

	uint32_t unShiftingMask = 0xFF'FF'FF'00;
	uint32_t unDestCopy = unDestinationRegister;

	while (unShiftingMask) {
	    unDestCopy &= unShiftingMask;

	    if (unDestCopy == 0 || unDestCopy == unShiftingMask)
		break;

	    nM++;
	    unShiftingMask <<= 8;
	}

	return ulResult;
    };

    switch (static_cast<ALU_OpCode>(usnOpcode)) {
    case ALU_OpCode::AND: unDestinationRegister = MSC(unDestinationRegister & unSourceRegister, true); break;
    case ALU_OpCode::EOR: unDestinationRegister = MSC(unDestinationRegister ^ unSourceRegister, true); break;
    case ALU_OpCode::LSL: unDestinationRegister = LSL(unDestinationRegister, unSourceRegister, true); break;
    case ALU_OpCode::LSR: unDestinationRegister = LSR(unDestinationRegister, unSourceRegister, true, false); break;
    case ALU_OpCode::ASR: unDestinationRegister = ASR(unDestinationRegister, unSourceRegister, true, false); break;
    case ALU_OpCode::ADC: unDestinationRegister = ADC(unDestinationRegister, unSourceRegister, true); break;
    case ALU_OpCode::SBC: unDestinationRegister = SBC(unDestinationRegister, unSourceRegister, true); break;
    case ALU_OpCode::ROR: unDestinationRegister = ROR(unDestinationRegister, unSourceRegister, true, false); break;
    case ALU_OpCode::TST:                         MSC(unDestinationRegister & unSourceRegister, true); break;
    case ALU_OpCode::NEG: unDestinationRegister = SUB(0, unSourceRegister, true); break;
    case ALU_OpCode::CMP:                         SUB(unDestinationRegister, unSourceRegister, true); break;
    case ALU_OpCode::CMN:                         ADD(unDestinationRegister, unSourceRegister, true); break;
    case ALU_OpCode::ORR: unDestinationRegister = MSC(unDestinationRegister | unSourceRegister, true); break;
    case ALU_OpCode::MUL: nM = 1;  unDestinationRegister = lamReducedMUL(); break;
    case ALU_OpCode::BIC: unDestinationRegister = MSC(unDestinationRegister & ~unSourceRegister, true); break;
    case ALU_OpCode::MVN: unDestinationRegister = MSC(~unSourceRegister, true); break;
    }

    // Idle if multiply
}