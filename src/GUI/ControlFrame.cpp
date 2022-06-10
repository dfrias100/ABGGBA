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

#include "ControlFrame.h"

ControlFrame::ControlFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size) {
    // TODO: Conduct business logic

    // Setup file menu
    // TODO: Add status bar text
    m_pwmFrameFileMenu = new wxMenu;
    m_pwmFrameFileMenu->Append(ControlFrameEventID::FileLoad, "&Load ROM");
    m_pwmFrameFileMenu->AppendSeparator();
    m_pwmFrameFileMenu->Append(wxID_ABOUT, "&About");
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
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ControlFrame::OnAbout, this, wxID_ABOUT);

    // TODO: Setup Emulator
}

void ControlFrame::AddConfig(std::shared_ptr<wxFileConfig> pwxfcAppConfig) {
    m_pwfcAppConfig = pwxfcAppConfig;
}

void ControlFrame::OnExit(wxCommandEvent& WXUNUSED(evt)) {
    Close(true);
}

void ControlFrame::OnAbout(wxCommandEvent& WXUNUSED(evt)) {
    // TODO: do most of this setup in another place (own class, init func, or somewhere else)
    // TODO: make those strings const

    wxDialog* pwdAboutDialog = new wxDialog(this, wxID_ANY, L"About ABGGBA");

    wxStaticText* pwstAppTitle = new wxStaticText(pwdAboutDialog, wxID_ANY, L"ABGGBA DEVEL", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    pwstAppTitle->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    wxStaticText* pwstDescriptionText = new wxStaticText(pwdAboutDialog, wxID_ANY, L"Nintendo Game Boy Advance emulator using wxWidgets and SDL2");
    wxStaticText* pwstCopyrightText = new wxStaticText(pwdAboutDialog, wxID_ANY, L"Copyright (C) 2022 Daniel Frias");
    wxHyperlinkCtrl* pwhlcGitHubRepoLink = new wxHyperlinkCtrl(pwdAboutDialog, wxID_ANY, L"GitHub Repository", L"https://github.com/dfrias100/ABGGBA");

    wxButton* pwbtnOkButton = new wxButton(pwdAboutDialog, wxID_OK, "&OK");

    wxBoxSizer* pwbsizDialogSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pwbsizButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pwbsizTopSectionSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* pwsbsizLicenseBoxSizer = new wxStaticBoxSizer(wxVERTICAL, pwdAboutDialog, L"GNU General Public License");

    wxTextCtrl* pwtcLicenseText = new wxTextCtrl(pwsbsizLicenseBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxSIZE_AUTO_WIDTH, 125),
						    wxTE_READONLY | wxTE_MULTILINE | wxTE_WORDWRAP);
    // TODO: This line is really long
    pwtcLicenseText->SetValue(L"This program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n");
    pwtcLicenseText->SetBackgroundColour(pwdAboutDialog->GetBackgroundColour());

    pwbsizTopSectionSizer->Add(pwstDescriptionText, 0, wxALIGN_LEFT | wxLEFT, 10);
    pwbsizTopSectionSizer->AddSpacer(5);
    pwbsizTopSectionSizer->Add(pwstCopyrightText, 0, wxALIGN_LEFT | wxLEFT, 10);
    pwbsizTopSectionSizer->AddSpacer(5);
    pwbsizTopSectionSizer->Add(pwhlcGitHubRepoLink, 0, wxALIGN_LEFT | wxLEFT, 10);

    pwsbsizLicenseBoxSizer->Add(pwtcLicenseText, 0, wxGROW);

    pwbsizButtonSizer->Add(pwbtnOkButton, 0, wxALL, 10);

    pwbsizDialogSizer->Add(pwstAppTitle, 0, wxALIGN_LEFT | wxALL, 10);
    pwbsizDialogSizer->Add(pwbsizTopSectionSizer);
    pwbsizDialogSizer->Add(pwsbsizLicenseBoxSizer, 0, wxGROW | wxLEFT | wxTOP | wxRIGHT, 10);
    pwbsizDialogSizer->AddStretchSpacer();
    pwbsizDialogSizer->Add(pwbsizButtonSizer, 0, wxALIGN_RIGHT);

    pwdAboutDialog->SetSize(wxSize(450, 350));
    pwdAboutDialog->SetSizer(pwbsizDialogSizer);

    pwbtnOkButton->SetFocusFromKbd();

    pwdAboutDialog->ShowModal();
}
