#ifndef CONTROL_FRAME_H
#define CONTROL_FRAME_H

#include "wxInclude.h"

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
};

#endif