#pragma once

constexpr hudtextparms_t g_LO3_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 10.0f, 2.0f, 4};

constexpr std::array<const char*, 5> g_LO3_HudText =
{
    "LIVE IN THREE RESTARTS",
    "LIVE IN THREE RESTARTS",
    "LIVE IN TWO RESTARTS",
    "LIVE ON NEXT RESTART",
    "LIVE GOOD LUCK & HAVE FUN"
};

constexpr std::array<const char*, 5> g_LO3_Message =
{
    "^4[%s]^1 %s: ^3Live in three restarts!",
    "^4[%s]^1 %s: ^3Live in three restarts!",
    "^4[%s]^1 %s: ^3Live in two restarts!",
    "^4[%s]^1 %s: ^3Live on next restart!",
    "^4[%s]^1 %s: ^Live! Good Luck & Have Fun!!"
};

constexpr std::array<const char*, 5> g_LO3_Sound =
{
    "spk \"fvox/blip, voice_off\"",
    "spk \"fvox/blip, three\"",
    "spk \"fvox/blip, two\"",
    "spk \"fvox/blip, one\"",
    "spk \"barney/letsgo\""
};

class CPugLO3
{
public:
    void Init();
    void RestartRound();

private:
    bool m_Run = false;
    int m_Restart = 0;
};

extern CPugLO3 gPugLO3;