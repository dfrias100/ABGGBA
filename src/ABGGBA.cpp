/*+==============================================================================
  File:      ABGGBA.cpp

  Summary:   Defines the methods of ABGGBA.

  Classes:   ABGGBA

  Functions: ABGGBA::OnInit(), ABGGBA::OnExit()

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

#include "ABGGBA.h"

bool ABGGBA::OnInit() {
    m_pControlFrame = new ControlFrame("ABGGBA", wxPoint(50, 50), wxSize(300, 300));
    m_pwfcAppConfig = std::make_shared<wxFileConfig>("ABGGBA", wxEmptyString, 
						    wxGetCwd() + L"\\ABGGBA.ini", wxEmptyString, wxCONFIG_USE_GLOBAL_FILE);
    m_pControlFrame->AddConfig(m_pwfcAppConfig);
    m_pControlFrame->Show(true);
    return true;
}

int ABGGBA::OnExit() {
    m_pwfcAppConfig->Flush();
    return 0;
}