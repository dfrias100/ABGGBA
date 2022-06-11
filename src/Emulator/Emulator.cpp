#include "Emulator.h"

Emulator::Emulator(ControlFrame* pParentWindow) {
    m_pRenderer = new Renderer();
    m_pParentWindow = pParentWindow;
}

Emulator::~Emulator() {
    StopEmulation();
    delete m_pRenderer;
}

void Emulator::StartEmulation() {
    m_bStart = true;
    m_bPaused = false;
    m_thEmulatorWorkerThread = std::thread(&Emulator::DoEmulation, this);
    m_pRenderer->StartWindowing();
}

void Emulator::PauseEmulation() {
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_bPaused = true;
}

void Emulator::UnPauseEmulation() {
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_bPaused = false;

    lkGuard.unlock();
    m_cvEmulatorWait.notify_one();
}

void Emulator::StopEmulation() {
    m_bStart = false;
    m_cvEmulatorWait.notify_one();

    if (m_thEmulatorWorkerThread.joinable()) {
	m_thEmulatorWorkerThread.join();
    }

    m_bPaused = false;

    m_pRenderer->StopWindowing();
}

bool Emulator::IsPaused() {
    return m_bPaused;
}

bool Emulator::IsRunning() {
    return m_thEmulatorWorkerThread.joinable();
}

void Emulator::DoEmulation() {
    uint32_t* paunDummyPixels = new uint32_t[240 * 160];
    memset(paunDummyPixels, 240 * 160 * sizeof(uint32_t), 0xFF);
    bool bSdlWindowWantedToClose = false;
    srand(time(0));

    m_pRenderer->AttachFramebuffer(paunDummyPixels);
    SDL_Event sdlEvent;

    while (m_bStart) {
	while (SDL_PollEvent(&sdlEvent)) {
	    if (sdlEvent.type == SDL_WINDOWEVENT &&
		sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
		m_bStart = false;
		bSdlWindowWantedToClose = true;
		printf("THREAD: closing sdl windows\n");
		break;
	    }
	}

	if (m_bPaused) {
	    PauseUntilNotify();
	} else {
	    constexpr int nSize = 240 * 160;
	    for (int i = 0; i < nSize; i++) {
		paunDummyPixels[i] = 0;
		paunDummyPixels[i] |=  rand() & 0xFF;
		paunDummyPixels[i] |= (rand() & 0xFF) << 8;
		paunDummyPixels[i] |= (rand() & 0xFF) << 16;
		paunDummyPixels[i] |=  0xFF000000;
	    }

	    m_pRenderer->Draw();
	}
    }

    if (bSdlWindowWantedToClose)
	m_pParentWindow->SendCloseEventToEmulator();

    delete[] paunDummyPixels;
}

void Emulator::PauseUntilNotify() {
    std::unique_lock<std::mutex> lkGuard(m_mtxEmulatorControl);
    m_cvEmulatorWait.wait(lkGuard, [&]() {
	return !m_bStart || !m_bPaused;
	});
}
