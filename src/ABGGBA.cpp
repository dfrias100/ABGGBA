#include "ABGGBA.h"

bool ABGGBA::OnInit() {
	// TODO: Load file config
	m_pControlFrame = new ControlFrame("ABGGBA", wxPoint(50, 50), wxSize(300, 300));
	m_pwfcAppConfig = std::make_shared<wxFileConfig>("ABGGBA", wxEmptyString, 
														wxGetCwd() + L"\\ABGGBA.ini", wxEmptyString, wxCONFIG_USE_GLOBAL_FILE);
	m_pControlFrame->AddConfig(m_pwfcAppConfig);
	m_pControlFrame->Show(true);
	return true;
}

int ABGGBA::OnExit() {
	// TODO: Flush file config
	m_pwfcAppConfig->Flush();
	return 0;
}