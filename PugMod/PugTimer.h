#pragma once

constexpr hudtextparms_t g_Timer_HudParam = {-1.0f, 0.18f, 2, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0.0f, 0.0f, 0.92f, 0.92f, 1};

class CPugtimer
{
public:
    void ServerActivate();
    void ServerDeactivate();

    void Init();
    void Stop();
    void RestartRound();
    void StartFrame();

private:
    bool m_Run;
    float m_NextFrame;
    time_t m_Time;
};

extern CPugtimer gPugTimer;
