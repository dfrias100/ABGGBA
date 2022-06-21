#include "ARM7TDMI.h"
#include "Instruction.h"

template <uint32_t Instruction>
constexpr ARM_Instruction ARM7TDMI::DecodeARM_Instruction() {
    if (Instruction == 0b0001'0010'0001) {

	return ARM_Instruction(ARM_Operation::BranchAndExchange, &ARM7TDMI::BranchAndExchange);

    } else if ((Instruction & 0b1111'1011'1111) == 0b0001'0000'1001) {

	return ARM_Instruction(ARM_Operation::SingleDataSwap, &ARM7TDMI::SingleDataSwap);

    } else if ((Instruction & 0b1111'1100'1111) == 0b0000'0000'1001) {

	return ARM_Instruction(ARM_Operation::Multiply, &ARM7TDMI::Multiply);

    } else if ((Instruction & 0b1111'1000'1111) == 0b0000'1000'1001) {

	return ARM_Instruction(ARM_Operation::MultiplyLong, &ARM7TDMI::MultiplyLong);

    } else if ((Instruction & 0b1110'0100'1001) == 0b0000'0000'1001) {

	if ((Instruction & 0x6) == 0x0) {
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstructionARM);
	}
	return ARM_Instruction(ARM_Operation::HalfwordDataTransferRegOff, &ARM7TDMI::HalfwordDataTransfer);

    } else if ((Instruction & 0b1110'0100'1001) == 0b0000'0100'1001) {

	if ((Instruction & 0x6) == 0x0) {
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstructionARM);
	}
	return ARM_Instruction(ARM_Operation::HalfwordDataTransferImmOff, &ARM7TDMI::HalfwordDataTransfer);

    } else if ((Instruction & 0b1111'0000'0000) == 0b1111'0000'0000) {

	return ARM_Instruction(ARM_Operation::SoftwareInterrupt, &ARM7TDMI::SoftwareInterruptARM);

    } else if ((Instruction & 0b1110'0000'0001) == 0b0110'0000'0001) {

	return ARM_Instruction(ARM_Operation::Undefined, &ARM7TDMI::UnimplementedInstructionARM);

    } else if ((Instruction & 0b1110'0000'0000) == 0b1010'0000'0000) {

	return ARM_Instruction(ARM_Operation::Branch, &ARM7TDMI::Branch);

    } else if ((Instruction & 0b1110'0000'0000) == 0b1000'0000'0000) {

	return ARM_Instruction(ARM_Operation::BlockDataTransfer, &ARM7TDMI::BlockDataTransfer);

    } else if ((Instruction & 0b1100'0000'0000) == 0b0100'0000'0000) {

	return ARM_Instruction(ARM_Operation::SingleDataTransfer, &ARM7TDMI::SingleDataTransfer);

    } else if ((Instruction & 0b1100'0000'0000) == 0b0000'0000'0000) {

	if ((Instruction & 0x190) == 0x100) {
	    return ARM_Instruction(ARM_Operation::PSR_Transfer, &ARM7TDMI::PSR_Transfer);
	}

	constexpr uint32_t nOpcode = Instruction & 0x1E0;
	constexpr uint32_t TST = 0b1000 == nOpcode;
	constexpr uint32_t TEQ = 0b1001 == nOpcode;
	constexpr uint32_t CMP = 0b1010 == nOpcode;
	constexpr uint32_t CMN = 0b1011 == nOpcode;

	if ((Instruction & 0x010) && (TST | TEQ | CMP | CMN))
	    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstructionARM);

	return ARM_Instruction(ARM_Operation::DataProcessing, &ARM7TDMI::DataProcessing);

    }

    return ARM_Instruction(ARM_Operation::UnsupportedArmInstruction, &ARM7TDMI::UnimplementedInstructionARM);
}

template <uint16_t Instruction>
constexpr THUMB_Instruction ARM7TDMI::DecodeTHUMB_Instruction() {
    if ((Instruction & 0b11'1111'1100) == 0b11'0111'1100) return THUMB_Instruction(THUMB_Operation::SoftwareInterrupt, &ARM7TDMI::SoftwareInterruptTHUMB);
    if ((Instruction & 0b11'1111'1100) == 0b10'1100'0000) return THUMB_Instruction(THUMB_Operation::AddOffToSP, &ARM7TDMI::AddOffToSP);
    if ((Instruction & 0b11'1101'1000) == 0b10'1101'0000) return THUMB_Instruction(THUMB_Operation::PushOrPopRegs, &ARM7TDMI::PushOrPopRegs);
    if ((Instruction & 0b11'1111'0000) == 0b01'0001'0000) {
	constexpr uint8_t ubyRegH1 = Instruction & 0x1;
	constexpr uint8_t ubyRegH2 = (Instruction >> 1) & 0x1;
	constexpr uint8_t ubyOpcode = (Instruction >> 2) & 0x3;

	if (ubyOpcode != 0b11 && ubyRegH1 == 0 && ubyRegH2 == 0) {
	    return THUMB_Instruction(THUMB_Operation::UnknownThumbInstruction, &ARM7TDMI::UnimplementedInstructionTHUMB);
	}

	if (ubyOpcode == 0b11 && ubyRegH2 == 1) {
	    return THUMB_Instruction(THUMB_Operation::UnknownThumbInstruction, &ARM7TDMI::UnimplementedInstructionTHUMB);
	}

	return THUMB_Instruction(THUMB_Operation::HiRegisterOperationsOrBranchExchange, &ARM7TDMI::HiRegisterOpsOrBranchExchange);
    }
    if ((Instruction & 0b11'1111'0000) == 0b01'0000'0000) return THUMB_Instruction(THUMB_Operation::ALU_Ops, &ARM7TDMI::ALU);
    if ((Instruction & 0b11'1110'0000) == 0b11'1000'0000) return THUMB_Instruction(THUMB_Operation::UnconditionalBranch, &ARM7TDMI::UnconditionalBranch);
    if ((Instruction & 0b11'1110'0000) == 0b01'0010'0000) return THUMB_Instruction(THUMB_Operation::PC_RelLoad, &ARM7TDMI::PC_RelLoad);
    if ((Instruction & 0b11'1110'0000) == 0b00'0110'0000) return THUMB_Instruction(THUMB_Operation::AddSubtract, &ARM7TDMI::AddSubtract);
    if ((Instruction & 0b11'1100'1000) == 0b01'0100'0000) return THUMB_Instruction(THUMB_Operation::LoadOrStoreWithRegOff, &ARM7TDMI::LoadOrStoreWithRegOff);
    if ((Instruction & 0b11'1100'1000) == 0b01'0100'1000) return THUMB_Instruction(THUMB_Operation::LoadOrStoreSignExtendedByteOrHalfWord, &ARM7TDMI::LoadOrStoreSignExtendedByteOrHalfWord);
    if ((Instruction & 0b11'1100'0000) == 0b11'1100'0000) return THUMB_Instruction(THUMB_Operation::LongBranchWithLink, &ARM7TDMI::LongBranchWithLink);
    if ((Instruction & 0b11'1100'0000) == 0b11'0100'0000) {
	uint32_t unCondition = (Instruction >> 2) & 0b1111;

	if (unCondition == 0xE || unCondition == 0xF) {
	    return THUMB_Instruction(THUMB_Operation::UnknownThumbInstruction, &ARM7TDMI::UnimplementedInstructionTHUMB);
	}

	return THUMB_Instruction(THUMB_Operation::ConditionalBranch, &ARM7TDMI::ConditionalBranch);
    }
    if ((Instruction & 0b11'1100'0000) == 0b11'0000'0000) return THUMB_Instruction(THUMB_Operation::MultipleLoadOrStore, &ARM7TDMI::MultipleLoadOrStore);
    if ((Instruction & 0b11'1100'0000) == 0b10'1000'0000) return THUMB_Instruction(THUMB_Operation::LoadAddress, &ARM7TDMI::LoadAddress);
    if ((Instruction & 0b11'1100'0000) == 0b10'0100'0000) return THUMB_Instruction(THUMB_Operation::SP_RelLoadOrStore, &ARM7TDMI::SP_RelLoadOrStore);
    if ((Instruction & 0b11'1100'0000) == 0b10'0000'0000) return THUMB_Instruction(THUMB_Operation::LoadOrStoreHalfWord, &ARM7TDMI::LoadOrStoreHalfWord);
    if ((Instruction & 0b11'1000'0000) == 0b01'1000'0000) return THUMB_Instruction(THUMB_Operation::LoadOrStoreWithImmOff, &ARM7TDMI::LoadOrStoreWithImmOff);
    if ((Instruction & 0b11'1000'0000) == 0b00'1000'0000) return THUMB_Instruction(THUMB_Operation::MoveCompareAddSubtractImm, &ARM7TDMI::MoveCompareAddSubtractImm);
    if ((Instruction & 0b11'1000'0000) == 0b00'0000'0000) return THUMB_Instruction(THUMB_Operation::MoveShiftedRegister, &ARM7TDMI::MoveShiftedRegister);

    return THUMB_Instruction(THUMB_Operation::UnknownThumbInstruction, &ARM7TDMI::UnimplementedInstructionTHUMB);
}


/*----------------------------------------------------------------------
    This bit of code comes from https://stackoverflow.com/a/47563100
  --------------------------------------------------------------------*/

template<std::size_t N>
struct num { static const constexpr auto value = N; };

template <class F, std::size_t... Is>
constexpr void for_(F func, std::index_sequence<Is...>) {
    (func(num<Is>{}), ...);
}

template <std::size_t N, typename F>
constexpr void for_(F func) {
    for_(func, std::make_index_sequence<N>());
}

constexpr void ARM7TDMI::GenerateARM_Tables(ARM_FunctionPointer aarmInstructionTable[0x1000]) {
    for_<0x1000>([&](auto i) {
	    aarmInstructionTable[i.value] = DecodeARM_Instruction<i.value>().armInstruction;
	});
}

constexpr void ARM7TDMI::GenerateTHUMB_Tables(THUMB_FunctionPointer atmbInstructionTable[0x400]) {
    for_<0x400>([&](auto i) {
	    atmbInstructionTable[i.value] = DecodeTHUMB_Instruction<i.value>().tmbInstruction;
	});
}

void ARM7TDMI::GenerateInstructionTables() {
    GenerateARM_Tables(m_aarmInstructionTable);
    GenerateTHUMB_Tables(m_atmbInstructionTable);
}