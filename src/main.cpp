/*+==============================================================================
  File:      main.cpp

  Summary:   The entry point of the emulator, here the SDL2 library will be init-
	     -ialized and wxEntry will be called at the end of the function.

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

#include "GUI/wxInclude.h"
#include "Core/ARM7TDMI/ARM7TDMI.h"
#include <SDL2/SDL.h>

#if defined(WIN32) && defined(_DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

int main(int argc, char* argv[]) {

#if defined(WIN32) && defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    ARM7TDMI::GenerateInstructionTables();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) != 0) {
	// TODO: make this a wx message box
	std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
	return 1;
    }

    return wxEntry(argc, argv);
}