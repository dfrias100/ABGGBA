/*+==============================================================================
  File:      MiscARM.cpp

  Summary:   Instructions of the ARM7TDMI that don't belong to a particular
	     category of instruction

  Classes:   ARM7TDMI

  Functions: ARM7TDMI::SoftwareInterruptARM, 
	     ARM7TDMI::UnimplementedInstructionARM

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

#include "../ARM7TDMI.h"
#include <iostream>
#include <iomanip>

void ARM7TDMI::SoftwareInterruptARM(uint32_t unInstruction) {

}

void ARM7TDMI::UnimplementedInstructionARM(uint32_t unInstruction) {
    std::cerr << "WARN: Unknown or undefined instruction " << std::hex << std::setw(8) << unInstruction <<
	" executed at PC = 0x" << std::hex << std::setw(8) << m_PC - 8 << std::endl;
}