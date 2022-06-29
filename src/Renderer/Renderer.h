/*+==============================================================================
  File:      Renderer.h

  Summary:   Defines the Renderer, SDL2 wrapper, class to handle windowing
	     and drawing

  Classes:   Renderer

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

#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <chrono>
#include <cmath>
#include <cstdint>

#include <SDL2/SDL.h>

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    Renderer

  Summary:  Class to wrap SDL2 functions into simple function calls

  Methods:  Renderer
	      Constructor.
	      Initializes the SDL window and texture to be used
	      as the framebuffer.
	    ~Renderer
	      Destructor.
	      Cleans up the SDL window data structures
	    StartWindowing
	      Sets the window to shown
	    StopWindowing
	      Sets the window to hidden
	    AttachFramebuffer
	      Attaches a pointer to the pixel buffer used by the
	      GBA components
	    Draw
	      Draws the pixel buffer
	    CapFramerate
	      Templated method to allow a fast way to cap the
	      framerate
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class Renderer {
public:
    Renderer();
    ~Renderer();

    void StartWindowing();
    void StopWindowing();
    void AttachFramebuffer(uint32_t* pGfxArray);
    void Draw();

    template <int Framerate>
    void CapFramerate(std::chrono::steady_clock::time_point timStart);

    uint32_t m_nWinSizeMultiplier = 4;
private:
    SDL_Window* m_pSdlWindow;
    SDL_Renderer* m_pSdlRenderer;
    SDL_Texture* m_pSdlFramebuffer;

    uint32_t* m_pGfxArray;
    uint32_t* m_punPixels;
    const int m_nPITCH = 240 * sizeof(uint32_t);
};

#endif

