#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "ARM7TDMI.h"

class ARM7TDMI;

typedef void (ARM7TDMI::* ARM_FunctionPointer)(uint32_t unInstruction);
typedef void (ARM7TDMI::* THUMB_FunctionPointer)(uint16_t usnInstruction);

enum class ARM_Operation {
    DataProcessing,
    PSR_Transfer,
    Multiply,
    MultiplyLong,
    SingleDataSwap,
    BranchAndExchange,
    HalfwordDataTransferRegOff,
    HalfwordDataTransferImmOff,
    SingleDataTransfer,
    Undefined,
    BlockDataTransfer,
    Branch,
    SoftwareInterrupt,
    UnsupportedArmInstruction
};

enum class THUMB_Operation {
    MoveShiftedRegister,
    AddSubtract,
    MoveCompareAddSubtractImm,
    ALUOps,
    HiRegisterOperationsOrBranchExchange,
    PCRelLoad,
    LoadOrStoreWithRegOff,
    LoadOrStoreSignExtendedByteOrHalfWord,
    LoadOrStoreWithImmOff,
    LoadOrStoreHalfword,
    SPRelLoadOrStore,
    LoadAddress,
    AddOffToSP,
    PushOrPopRegs,
    MultipleLoadOrStore,
    ConditionalBranch,
    SoftwareInterrupt,
    UnconditionalBranch,
    LongBranchWithLink,
    UnknownThumbInstruction
};

struct ARM_Instruction {
    ARM_Operation armOp;
    ARM_FunctionPointer armInstruction;
    ARM_Instruction();
    ARM_Instruction(ARM_Operation _armOp, ARM_FunctionPointer _armInstruction) :
	armOp(armOp), armInstruction(_armInstruction) { };
};

struct THUMB_Instruction {
    THUMB_Operation tmbOp;
    THUMB_FunctionPointer tmbInstruction;
};

#endif