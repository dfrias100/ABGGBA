/*+==============================================================================
  File:      ControlFrame.h

  Summary:   Defines the methods of the ControlFrame class.

  Classes:   ControlFrame

  Functions: ControlFrame::ControlFrame, ControlFrame::AddConfig,   
             ControlFrame::OnExit, ControlFrame::OnAbout, 
	     ControlFrame::InitAboutDialog

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

#include <cstdint>

#include "ControlFrame.h"
#include "../Renderer/Renderer.h"

const wxString ControlFrame::m_wszWINDOW_TITLE = L"About ABGGBA";
const wxString ControlFrame::m_wszAPP_TITLE = L"ABGGBA";
const wxString ControlFrame::m_wszAPP_VERSION = L"DEVEL";
const wxString ControlFrame::m_wszAPP_DESCRIPTION = L"Nintendo Game Boy Advance emulator using wxWidgets and SDL2";
const wxString ControlFrame::m_wszCOPYRIGHT_TEXT = L"Copyright (C) 2022 Daniel Frias";
const wxString ControlFrame::m_wszGITHUB_DESC = L"GitHub Repository";
const wxString ControlFrame::m_wszGITHUB_REPO_LINK = L"https://github.com/dfrias100/ABGGBA";
const wxString ControlFrame::m_wszLICENSE_BOX_LABEL = L"GNU General Public License";
const wxString ControlFrame::m_wszLICENSE_TEXT = L"This program is free software; you can redistribute it and/or\n" 
						L"modify it under the terms of the GNU General Public License\n" 
						L"as published by the Free Software Foundation; either version 2\n" 
						L"of the License, or (at your option) any later version.\n\n" 
						L"This program is distributed in the hope that it will be useful, \n" 
						L"but WITHOUT ANY WARRANTY; without even the implied warranty of\n" 
						L"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n" 
						L"GNU General Public License for more details.\n\n" 
						L"You should have received a copy of the GNU General Public License\n"
						L"along with this program; if not, write to the Free Software\n" 
						L"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.\n";

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

    // Initialize about dialog
    InitAboutDialog();

    // Add these to the frame
    SetMenuBar(m_pwmbFrameMenuBar);
    CreateStatusBar();
    SetStatusText("Welcome to ABGGBA!");

    // TODO: Add all bind calls
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ControlFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ControlFrame::OnAbout, this, wxID_ABOUT);

    // TODO: Setup Emulator
}

void ControlFrame::AddConfig(std::shared_ptr<wxFileConfig>& pwxfcAppConfig) {
    m_pwfcAppConfig = pwxfcAppConfig;
}

void ControlFrame::OnExit(wxCommandEvent& WXUNUSED(evt)) {
    Close(true);
}

void ControlFrame::OnAbout(wxCommandEvent& WXUNUSED(evt)) {
    m_pwdAboutDialog->ShowModal();
}

void ControlFrame::InitAboutDialog() {
    /* -------------------------------------------------------------------------------------------------
       Don't have to worry about deletion, however we will have to pass a raw pointer to its children.
       The children don't do anything with the parent object allocation, so it's safe to pass it as
       a raw pointer.
    ------------------------------------------------------------------------------------------------- */
    m_pwdAboutDialog = std::make_unique<wxDialog>(this, wxID_ANY, m_wszWINDOW_TITLE);

    // Add large, bold text, to the dialog
    wxStaticText* pwstAppTitle = new wxStaticText(m_pwdAboutDialog.get(), wxID_ANY, m_wszAPP_TITLE + L" " + m_wszAPP_VERSION, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    pwstAppTitle->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // These ones wont be as large'and we'll create them using the constants we defined earlier
    wxStaticText* pwstDescriptionText = new wxStaticText(m_pwdAboutDialog.get(), wxID_ANY, m_wszAPP_DESCRIPTION);
    wxStaticText* pwstCopyrightText = new wxStaticText(m_pwdAboutDialog.get(), wxID_ANY, m_wszCOPYRIGHT_TEXT);
    wxHyperlinkCtrl* pwhlcGitHubRepoLink = new wxHyperlinkCtrl(m_pwdAboutDialog.get(), wxID_ANY, m_wszGITHUB_DESC, m_wszGITHUB_REPO_LINK);

    // Dialogs don't come with the OK button
    wxButton* pwbtnOkButton = new wxButton(m_pwdAboutDialog.get(), wxID_OK, "&OK");

    /* ----------------------------------------------------------------------------------------
       We'll need the use of a couple of sizers:
           - The dialog sizer will be the main sizer to vertically place all the sub-sizers
           - Button sizer will just align the button to the lower right
           - The top section sizer will hold all the text
           - The license box sizer is a special sizer for the static box, it will allow us
             to place a text control inside of it.
    ---------------------------------------------------------------------------------------- */
    wxBoxSizer* pwbsizDialogSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pwbsizButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pwbsizTopSectionSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* pwsbsizLicenseBoxSizer = new wxStaticBoxSizer(wxVERTICAL, m_pwdAboutDialog.get(), m_wszLICENSE_BOX_LABEL);

    // This would have been defined earlier, but it depends on the static box inside the static box sizer
    wxTextCtrl* pwtcLicenseText = new wxTextCtrl(pwsbsizLicenseBoxSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(wxSIZE_AUTO_WIDTH, 125),
	wxTE_READONLY | wxTE_MULTILINE | wxTE_WORDWRAP);
    // ^ These flags above let us have the text be copyable and scrollable (otherwise we'd have to do a lot more setup)
    pwtcLicenseText->SetValue(m_wszLICENSE_TEXT);

    // Fake transparency :)
    pwtcLicenseText->SetBackgroundColour(m_pwdAboutDialog->GetBackgroundColour());

    // Add these in the order that they will be seen, from the most nested to the top
    pwbsizTopSectionSizer->Add(pwstDescriptionText, 0, wxALIGN_LEFT | wxLEFT, 10);
    pwbsizTopSectionSizer->AddSpacer(5);
    pwbsizTopSectionSizer->Add(pwstCopyrightText,   0, wxALIGN_LEFT | wxLEFT, 10);
    pwbsizTopSectionSizer->AddSpacer(5);
    pwbsizTopSectionSizer->Add(pwhlcGitHubRepoLink, 0, wxALIGN_LEFT | wxLEFT, 10);

    // Add our license text to to the box sizer
    pwsbsizLicenseBoxSizer->Add(pwtcLicenseText, 0, wxGROW);

    // Add our button
    pwbsizButtonSizer->Add(pwbtnOkButton, 0, wxALL, 10);

    // Add the sizers and objects from top to bottom
    pwbsizDialogSizer->Add(pwstAppTitle, 0, wxALIGN_LEFT | wxALL, 10);
    pwbsizDialogSizer->Add(pwbsizTopSectionSizer);
    pwbsizDialogSizer->Add(pwsbsizLicenseBoxSizer, 0, wxGROW | wxLEFT | wxTOP | wxRIGHT, 10);
    // Push the button all the way to the bottom with this spacer
    pwbsizDialogSizer->AddStretchSpacer();
    pwbsizDialogSizer->Add(pwbsizButtonSizer, 0, wxALIGN_RIGHT);

    // Set the size and sizer, finally
    m_pwdAboutDialog->SetSize(wxSize(450, 350));
    m_pwdAboutDialog->SetSizer(pwbsizDialogSizer);

    // Set the keyboard focus to the OK button
    pwbtnOkButton->SetFocusFromKbd();

    // All we have to do now is call ShowModal() from another function
}
