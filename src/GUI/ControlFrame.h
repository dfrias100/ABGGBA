//    ABGGBA: Nintendo Game Boy Advance emulator using wxWidgets and SDL2
//    Copyright(C) 2022  Daniel Frias
//
//    This program is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 2
//    of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef CONTROL_FRAME_H
#define CONTROL_FRAME_H

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

class ControlFrame : public wxFrame {
public:
    ControlFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void AddConfig(std::shared_ptr<wxFileConfig> pwxfcAppConfig);
private:
    std::shared_ptr<wxFileConfig> m_pwfcAppConfig;

    wxMenuBar* m_pwmbFrameMenuBar;

    wxMenu* m_pwmFrameFileMenu;
    wxMenu* m_pwmFrameControlMenu;

    /*void OnStart(wxCommandEvent& evt);
    void OnPause(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);*/
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);
};

#endif