#include "../ARM7TDMI.h"
#include <iostream>
#include <iomanip>

void ARM7TDMI::SoftwareInterruptARM(uint32_t unInstruction) {

}

void ARM7TDMI::UnimplementedInstructionARM(uint32_t unInstruction) {
    std::cerr << "WARN: Unknown or undefined instruction " << std::hex << std::setw(8) << unInstruction <<
	" executed at PC = 0x" << std::hex << std::setw(8) << m_PC - 8 << std::endl;
}