/*+==============================================================================
  File:      ABGGBA.h

  Summary:   Defines the ABGGBA class to be used as the wxEntry point

  Classes:   ABGGBA

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

#ifndef ABGGBA_H
#define ABGGBA_H

#include "GUI/wxInclude.h"
#include "GUI/ControlFrame.h"

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    ABGGBA

  Summary:  wxApp-derived class to facilitate the loading of the GUIs
            and config

  Methods:  OnInit
              Setup the GUI and load the config file
            OnExit
              Flushes the config file changes to storage
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class ABGGBA : public wxApp {
    ControlFrame* m_pControlFrame;
    std::shared_ptr<wxFileConfig> m_pwfcAppConfig;
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;
};

wxIMPLEMENT_APP_NO_MAIN(ABGGBA);

#endif