#pragma once

constexpr hudtextparms_t g_WARMUP_HudTimer = {-1.0f, 0.20f, 0, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0.0f, 0.0f, 0.53f, 0.53f, 1};

class CPugtimer
{
public:
	void Init();
	void Stop();
	void Task();

private:
	time_t m_Time;
};

extern CPugtimer gPugTimer;
