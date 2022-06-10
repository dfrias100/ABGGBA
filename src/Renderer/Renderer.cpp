/*+==============================================================================
  File:      Renderer.cpp

  Summary:   Defines the methods of the Renderer class

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

#include <iostream>
#include "Renderer.h"

Renderer::Renderer() {
    m_pSdlWindow = SDL_CreateWindow("ABGGBA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 3 * 240, 3 * 160, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    m_pSdlRenderer = SDL_CreateRenderer(m_pSdlWindow, -1, SDL_RENDERER_ACCELERATED);
    m_pSdlFramebuffer = SDL_CreateTexture(m_pSdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 240, 160);
}

Renderer::~Renderer() {
    SDL_DestroyWindow(m_pSdlWindow);
    SDL_DestroyRenderer(m_pSdlRenderer);
    SDL_DestroyTexture(m_pSdlFramebuffer);
}

void Renderer::StartWindowing() {
    SDL_ShowWindow(m_pSdlWindow);
}

void Renderer::StopWindowing() {
    SDL_HideWindow(m_pSdlWindow);
}

void Renderer::AttachFramebuffer(uint32_t*& pGfxArray) {
    m_pGfxArray = pGfxArray;
}

void Renderer::Draw() {
    SDL_LockTexture(m_pSdlFramebuffer, NULL, (void**) &punPixels, const_cast<int*>(&m_nPITCH));
    memcpy(punPixels, m_pGfxArray, 240 * 160 * sizeof(uint32_t));
    SDL_UnlockTexture(m_pSdlFramebuffer);

    SDL_RenderClear(m_pSdlRenderer);
    SDL_RenderCopy(m_pSdlRenderer, m_pSdlFramebuffer, NULL, NULL);
    SDL_RenderPresent(m_pSdlRenderer);
}
