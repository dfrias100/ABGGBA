#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "ARM7TDMI.h"

class ARM7TDMI;

typedef void (ARM7TDMI::* ARM_FunctionPointer)(uint32_t unInstruction);
typedef void (ARM7TDMI::* THUMB_FunctionPointer)(uint16_t usnInstruction);

enum class ConditionField {
    EQ = 0b0000,
    NE = 0b0001,
    CS = 0b0010,
    CC = 0b0011,
    MI = 0b0100,
    PL = 0b0101,
    VS = 0b0110,
    VC = 0b0111,
    HI = 0b1000,
    LS = 0b1001,
    GE = 0b1010,
    LT = 0b1011,
    GT = 0b1100,
    LE = 0b1101,
    AL = 0b1110
};

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
    ALU_Ops,
    HiRegisterOperationsOrBranchExchange,
    PC_RelLoad,
    LoadOrStoreWithRegOff,
    LoadOrStoreSignExtendedByteOrHalfWord,
    LoadOrStoreWithImmOff,
    LoadOrStoreHalfword,
    SP_RelLoadOrStore,
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

enum class DataProcessingOpCode {
    AND = 0b0000,
    EOR = 0b0001,
    SUB = 0b0010,
    RSB = 0b0011,
    ADD = 0b0100,
    ADC = 0b0101,
    SBC = 0b0110,
    RSC = 0b0111,
    TST = 0b1000,
    TEQ = 0b1001,
    CMP = 0b1010,
    CMN = 0b1011,
    ORR = 0b1100,
    MOV = 0b1101,
    BIC = 0b1110,
    MVN = 0b1111
};

struct ARM_Instruction {
    ARM_Operation armOp;
    ARM_FunctionPointer armInstruction;
    ARM_Instruction();
    ARM_Instruction(ARM_Operation _armOp, ARM_FunctionPointer _armInstruction) :
	armOp(_armOp), armInstruction(_armInstruction) { };
};

struct THUMB_Instruction {
    THUMB_Operation tmbOp;
    THUMB_FunctionPointer tmbInstruction;
};

#endif