#ifndef EMULATOR_H
#define EMULATOR_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../Renderer/Renderer.h";
#include "../GUI/ControlFrame.h"

class ControlFrame;

class Emulator {
public:
    Emulator(ControlFrame* pParentWindow);
    ~Emulator();

    void StartEmulation();
    void PauseEmulation();
    void UnPauseEmulation();
    void StopEmulation();

    bool IsPaused();
    bool IsRunning();
private:
    std::thread m_thEmulatorWorkerThread;
    std::mutex m_mtxEmulatorControl;
    std::condition_variable m_cvEmulatorWait;

    Renderer* m_pRenderer;
    ControlFrame* m_pParentWindow;

    bool m_bStart = false;
    bool m_bPaused = false;

    void DoEmulation();
    void PauseUntilNotify();
};

#endif