#include "ARM7TDMI.h"
#include "Instruction.h"

constexpr inline uint16_t ARM7TDMI::HashArmOpcode(uint32_t unArmOpcode) {
    return ((unArmOpcode & 0x0FF00000) >> 16) | ((unArmOpcode & 0xF0) >> 4);
}

template <uint32_t Instruction>
constexpr ARM_Instruction ARM7TDMI::DecodeARMInstruction() {
    constexpr uint16_t usnArmHash = HashArmOpcode(Instruction);
    if (usnArmHash == 0b0001'0010'0001) {

	return ARM_Instruction(ARM_Operation::BranchAndExchange, &ARM7TDMI::BranchAndExchange);

    } else if ((usnArmHash & 0b1111'1011'1111) == 0b0001'0000'1001) {

	return ARM_Instruction(ARM_Operation::SingleDataSwap, &ARM7TDMI::SingleDataSwap);

    } else if ((usnArmHash & 0b1111'1100'1111) == 0b0000'0000'1001) {

	return ARM_Instruction(ARM_Operation::Multiply, &ARM7TDMI::Multiply);

    } else if ((usnArmHash & 0b1111'1000'1111) == 0b0000'1000'1001) {

	return ARM_Instruction(ARM_Operation::MultiplyLong, &ARM7TDMI::MultiplyLong);

    } else if ((usnArmHash & 0b1110'0100'1001) == 0b0000'0000'1001) {

	if ((usnArmHash & 0x6) == 0x0) {
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstruction);
	}
	return ARM_Instruction(ARM_Operation::HalfwordDataTransferRegOff, &ARM7TDMI::HalfwordDataTransfer);

    } else if ((usnArmHash & 0b1110'0100'1001) == 0b0000'0100'1001) {

	if ((usnArmHash & 0x6) == 0x0) {
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstruction);
	}
	return ARM_Instruction(ARM_Operation::HalfwordDataTransferImmOff, &ARM7TDMI::HalfwordDataTransfer);

    } else if ((usnArmHash & 0b1111'0000'0000) == 0b1111'0000'0000) {

	return ARM_Instruction(ARM_Operation::SoftwareInterrupt, &ARM7TDMI::SoftwareInterrupt);

    } else if ((usnArmHash & 0b1110'0000'0001) == 0b0110'0000'0001) {

	return ARM_Instruction(ARM_Operation::Undefined, &ARM7TDMI::UnimplementedInstruction);

    } else if ((usnArmHash & 0b1110'0000'0000) == 0b1010'0000'0000) {

	return ARM_Instruction(ARM_Operation::Branch, &ARM7TDMI::Branch);

    } else if ((usnArmHash & 0b1110'0000'0000) == 0b1000'0000'0000) {

	return ARM_Instruction(ARM_Operation::BlockDataTransfer, &ARM7TDMI::BlockDataTransfer);

    } else if ((usnArmHash & 0b1100'0000'0000) == 0b0100'0000'0000) {

	return ARM_Instruction(ARM_Operation::SingleDataTransfer, &ARM7TDMI::SingleDataTransfer);

    } else if ((usnArmHash & 0b1100'0000'0000) == 0b0000'0000'0000) {

	if ((usnArmHash & 0x190) == 0x100) {
	    return ARM_Instruction(ARM_Operation::PSR_Transfer, &ARM7TDMI::PSR_Transfer);
	}

	constexpr uint32_t nOpcode = usnArmHash & 0x1E0;
	constexpr uint32_t TST = 0b1000 == nOpcode;
	constexpr uint32_t TEQ = 0b1001 == nOpcode;
	constexpr uint32_t CMP = 0b1010 == nOpcode;
	constexpr uint32_t CMN = 0b1011 == nOpcode;

	if ((usnArmHash & 0x010) && (TST | TEQ | CMP | CMN))
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstruction);

	return ARM_Instruction(ARM_Operation::DataProcessing, &ARM7TDMI::DataProcessing);

    }

    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstruction);
}