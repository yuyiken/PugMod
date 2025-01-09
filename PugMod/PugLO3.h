#pragma once

constexpr hudtextparms_t g_LO3_HudParam = {-1.0f, 0.2f, 2, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0.0f, 0.0f, 6.0f, 1.0f, 4};

constexpr std::array<const char*, 5> g_LO3_Message =
{
    "^4[%s]^1 Valendo em três restarts! ^3Fique atento !!",
    "^4[%s]^1 Valendo em três restarts!",
    "^4[%s]^1 Valendo em dois restarts!",
    "^4[%s]^1 Valendo no próximo restart!",
    "^4[%s]^1 A partida começou! Boa Sorte & Divirta-se!!"
};

constexpr std::array<const char*, 5> g_LO3_HudText =
{
    "..:: PREPARE-SE ::..",
    "..:: VALENDO EM TRÊS RESTARTS ::..",
    "..:: VALENDO EM DOIS RESTARTS ::..",
    "..:: VALENDO NO PRÓXIMO RESTART ::..",
    "..:: A PARTIDA COMEÇOU ::..\n..:: BOA SORTE & DIVIRTA-SE ::.."
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