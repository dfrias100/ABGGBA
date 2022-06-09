#ifndef ABGGBA_H
#define ABGGBA_H

#include "GUI/wxInclude.h"
#include "GUI/ControlFrame.h"

class ABGGBA : public wxApp {
	ControlFrame* m_pControlFrame;
	std::shared_ptr<wxFileConfig> m_pwfcAppConfig;
public:
	virtual bool OnInit() override;
	virtual int OnExit() override;
};

wxIMPLEMENT_APP_NO_MAIN(ABGGBA);

#endif