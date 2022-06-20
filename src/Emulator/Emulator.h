/*+==============================================================================
  File:      Emulator.h

  Summary:   Defines the Emulator class.

  Classes:   Emulator

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

#ifndef EMULATOR_H
#define EMULATOR_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "../Renderer/Renderer.h";
#include "../GUI/ControlFrame.h"
#include "../Core/GBA/GBA.h"

class ControlFrame;

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    Emulator

  Summary:  wxFrame-derived class to facilitate the functions of the
	    GUI and to manage the state of emulation.

  Methods:  Emulator
	      Constructor.
	      Allocates the renderer and takes in a ControlFrame
	      pointer so that the emulator thread can call to it
	    ~Emulator
	      Destructor.
	      Stops a running thread (if it is running) and de-
	      allocates the renderer
	    StartEmulation
	      Creates a thread that conducts the emulation loop
	    PauseEmulation
	      Causes the thread to wait until the condition
	      variable condition is true
	    UnPauseEmulation
	      Wakes up the emulation thread to continue
	    IsPaused
	      Returns the paused boolean
	    IsRunning
	      Returns a boolean denoting if the thread
	      is joinable
	    DoEmulation
	      See the .cpp file detailing this method
	    PauseUntilNotify
	      Wait until the condition variable condition
	      is met
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class Emulator {
public:
    Emulator(ControlFrame* pParentWindow);
    ~Emulator();

    void StartEmulation();
    void PauseEmulation();
    void UnPauseEmulation();
    void StopEmulation();

    bool IsPaused();
    bool IsRunning();
private:
    std::thread m_thEmulatorWorkerThread;
    std::mutex m_mtxEmulatorControl;
    std::condition_variable m_cvEmulatorWait;

    Renderer* m_pRenderer;
    ControlFrame* m_pParentWindow;
    std::chrono::steady_clock::time_point m_timStart;

    GBA* m_aGba;

    bool m_bStart = false;
    bool m_bPaused = false;

    void DoEmulation();
    void PauseUntilNotify();
};

#endif