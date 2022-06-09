#include "ControlFrame.h"

ControlFrame::ControlFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size) {
    // TODO: Conduct business logic

    // Setup file menu
    // TODO: Add status bar text
    m_pwmFrameFileMenu = new wxMenu;
    m_pwmFrameFileMenu->Append(ControlFrameEventID::FileLoad, "&Load ROM");
    m_pwmFrameFileMenu->AppendSeparator();
    m_pwmFrameFileMenu->Append(wxID_EXIT);


    // Setup control menu
    // TODO: Add status bar text
    m_pwmFrameControlMenu = new wxMenu;
    m_pwmFrameControlMenu->Append(ControlFrameEventID::StartEmu, "&Start");
    m_pwmFrameControlMenu->Append(ControlFrameEventID::PauseEmu, "&Pause");
    m_pwmFrameControlMenu->Append(ControlFrameEventID::StopEmu, "&Stop");

    // Disable pause and stop as no ROM has been loaded here
    m_pwmFrameControlMenu->Enable(ControlFrameEventID::PauseEmu, false);
    m_pwmFrameControlMenu->Enable(ControlFrameEventID::StopEmu, false);

    m_pwmbFrameMenuBar = new wxMenuBar;
    m_pwmbFrameMenuBar->Append(m_pwmFrameFileMenu, "&File");
    m_pwmbFrameMenuBar->Append(m_pwmFrameControlMenu, "&Emulation");

    // Add these to the frame
    SetMenuBar(m_pwmbFrameMenuBar);
    CreateStatusBar();
    SetStatusText("Welcome to ABGGBA!");

    // TODO: Add bind calls
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ControlFrame::OnExit, this, wxID_EXIT);

    // TODO: Setup Emulator
}

void ControlFrame::AddConfig(std::shared_ptr<wxFileConfig> pwxfcAppConfig) {
    m_pwfcAppConfig = pwxfcAppConfig;
}

void ControlFrame::OnExit(wxCommandEvent& WXUNUSED(evt)) {
    Close(true);
}
