/*+==============================================================================
  File:      Emulator.cpp

  Summary:   Defines the methods of the Emulator class.

  Classes:   Emulator

  Functions: Emulator::StartEmulation, Emulator::PauseEmulation, 
	     Emulator::UnPauseEmulation, Emulator::StopEmulation 
	     Emulator::IsPaused, Emulator::IsRunning, 
	     Emulator::DoEmulation, Emulator::PauseUntilNotify

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

#include "Emulator.h"

Emulator::Emulator(ControlFrame* pParentWindow) {
    m_pRenderer = new Renderer();
    m_pParentWindow = pParentWindow;

    m_aGba = new GBA();
    m_pRenderer->AttachFramebuffer(m_aGba->GetGraphicsArrayPointer());
}

Emulator::~Emulator() {
    StopEmulation();
    delete m_aGba;
    delete m_pRenderer;
}

void Emulator::StartEmulation() {
    m_bStart = true;
    m_bPaused = false;
    m_thEmulatorWorkerThread = std::thread(&Emulator::DoEmulation, this);

    // The renderer starts with no window shown, start it here
    m_pRenderer->StartWindowing();
}

void Emulator::PauseEmulation() {
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_bPaused = true;
}

void Emulator::UnPauseEmulation() {
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_bPaused = false;

    lkGuard.unlock();
    m_cvEmulatorWait.notify_one();
}

void Emulator::StopEmulation() {
    /*------------------------------------------
       Set it to false and notify the condition
       variable in case the thread was asleep
    ------------------------------------------*/
    m_bStart = false;
    m_cvEmulatorWait.notify_one();

    if (m_thEmulatorWorkerThread.joinable()) {
	m_thEmulatorWorkerThread.join();
    }

    // Set the bools as they were at start
    m_bPaused = false;

    // Hide the window
    m_pRenderer->StopWindowing();
}

bool Emulator::IsPaused() {
    return m_bPaused;
}

bool Emulator::IsRunning() {
    return m_thEmulatorWorkerThread.joinable();
}

// This is currently a "dummy" method
void Emulator::DoEmulation() {
    bool bSdlWindowWantedToClose = false;
    SDL_Event sdlEvent;

    while (m_bStart) {
	m_timStart = std::chrono::steady_clock::now();
	// TODO: make this its own function
	while (SDL_PollEvent(&sdlEvent)) {
	    if (sdlEvent.type == SDL_WINDOWEVENT &&
		sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
		m_bStart = false;
		bSdlWindowWantedToClose = true;
		break;
	    }
	}

	if (m_bPaused) {
	    PauseUntilNotify();
	    m_timStart = std::chrono::steady_clock::now();
	} else {
	    m_aGba->RunUntilFrame();
	    m_pRenderer->Draw();
	}

	m_pRenderer->CapFramerate<60>(m_timStart);
    }

    if (bSdlWindowWantedToClose)
	m_pParentWindow->SendCloseEventToEmulator();
}


void Emulator::PauseUntilNotify() {
    // Wait until the emulator is not started anymore or if it is
    // out of the pause
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_cvEmulatorWait.wait(lkGuard, [&]() {
	return !m_bStart || !m_bPaused;
	});
}

template <int Framerate>
void Renderer::CapFramerate(std::chrono::steady_clock::time_point timStart) {
    constexpr float fFrametime = 1e9f / Framerate;
    constexpr auto durFrametime = std::chrono::nanoseconds((uint64_t)fFrametime);
    auto timEnd = timStart + durFrametime;
    while (std::chrono::steady_clock::now() < timEnd) {}
}