/*+==============================================================================
  File:      ControlFrame.h

  Summary:   Defines the ControlFrame class and the ControlEventID enum.

  Classes:   ControlFrame

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

#ifndef CONTROL_FRAME_H
#define CONTROL_FRAME_H

#include <memory>

#include "wxInclude.h"
#include <wx/hyperlink.h>
#include <wx/statbox.h>
#include <wx/scrolwin.h>
// TODO: add this to wxInclude header
#include <wx/textctrl.h>

enum ControlFrameEventID {
    FileLoad = 1,
    StartEmu = 2,
    PauseEmu = 3,
    StopEmu = 4
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    ControlFrame

  Summary:  wxFrame-derived class to facilitate the functions of the
            GUI and to manage the state of emulation.

  Methods:  ControlFrame
              Constructor.
              Allocates the GUI and sets up its layout with default
              configuration as well as initializes the Emulator class.
            OnExit
              Catches the "Quit" button event in the GUI.
            OnAbout
              Displays the about wxDialog
            InitAboutDialog
              Sets up the data structures to display the about dialog.
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class ControlFrame : public wxFrame {
public:
    ControlFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void AddConfig(std::shared_ptr<wxFileConfig>& pwxfcAppConfig);
private:
    // String constants for about dialog
    static const wxString m_wszWINDOW_TITLE;
    static const wxString m_wszAPP_TITLE;
    static const wxString m_wszAPP_VERSION;
    static const wxString m_wszAPP_DESCRIPTION;
    static const wxString m_wszCOPYRIGHT_TEXT;
    static const wxString m_wszGITHUB_DESC;
    static const wxString m_wszGITHUB_REPO_LINK;
    static const wxString m_wszLICENSE_BOX_LABEL;
    static const wxString m_wszLICENSE_TEXT;

    std::shared_ptr<wxFileConfig> m_pwfcAppConfig;
    std::unique_ptr<wxDialog> m_pwdAboutDialog;

    wxMenuBar* m_pwmbFrameMenuBar;

    wxMenu* m_pwmFrameFileMenu;
    wxMenu* m_pwmFrameControlMenu;

    /*void OnStart(wxCommandEvent& evt);
    void OnPause(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);*/
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);

    void InitAboutDialog();
};

#endif