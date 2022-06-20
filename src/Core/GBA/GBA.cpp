/*+==============================================================================
  File:      GBA.cpp

  Summary:   Defines the GBA class' methods.

  Classes:   GBA

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

#include "GBA.h"

GBA::GBA() {
    m_aCpu = new ARM7TDMI();
}

GBA::~GBA() {
    delete m_aCpu;
}

void GBA::RunFor(uint64_t ulCycles) {
    for (int i = 0; i < ulCycles; i++)
	m_aCpu->Clock();
}
