#pragma once

constexpr hudtextparms_t g_LO3_HudParam = {-1.0f, 0.20f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 10.0f, 2.0f, 4};

constexpr std::array<const char*, 5> g_LO3_HudText =
{
    "LIVE EM TRÊS RESTARTS",
    "LIVE EM TRÊS RESTARTS",
    "LIVE EM DOIS RESTARTS",
    "LIVE NO PRÓXIMO RESTART",
    "BOA SORTE & DIVIRTA-SE"
};

constexpr std::array<const char*, 5> g_LO3_Message =
{
    "^4[%s]^1 %s: ^3Live em três restarts!",
    "^4[%s]^1 %s: ^3Live em três restarts!",
    "^4[%s]^1 %s: ^3Live em dois restarts!",
    "^4[%s]^1 %s: ^3Live no próximo restart!",
    "^4[%s]^1 %s: ^3Live! Boa Sorte & Divirta-se!!"
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